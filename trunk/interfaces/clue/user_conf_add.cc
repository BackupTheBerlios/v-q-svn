/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "cvq.hpp"
#include "main.hpp"

#include <unistd.h>

#include <exception>
#include <iostream>
#include <cerrno>
#include <deque>
#include <sstream>

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [options] (e-mail type value){1,}"<<endl
		<<"-q\tquiet mode, only first e-mail is processed"<<endl
		<<endl
		<<"Add options assiociated with mailboxes, for example autoresponse"<<endl
		<<"message, redirection address, etc."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			bool quiet=false;
			while( (opt=getopt(ac,av,"qf:h")) != -1 ) {
					switch(opt) {
					case 'q':
							quiet=true;
							break;
					default:		
					case '?':
					case 'h':
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;
			if( 1 > ac || ac % 3 ) {
					usage();
					return(1);
			}

			cvq *vq(cvq::alloc());

			uint8_t ret;
			char *atpos=NULL;

			if(quiet) ac=3;
			
			cvq::udot_info ui;
			istringstream type;
			int typeval;
			for(int i=0; i < ac; i+=3 ) {
					atpos = rindex(av[i], '@');
					if(!quiet) cout<<av[i]<<": ";
					if(atpos) {
							*atpos = '\0';
							atpos++;
							type.str(av[i+1]);
							type>>typeval;

							if(!type) {
									if( !quiet ) {
											cout<<"Invalid type: "<<av[i+1]<<endl;
											continue;
									}
									else return 1;
							}
							if( ! vq->udot_sup_is(static_cast<cvq::udot_type>(typeval)) ) {
									if( !quiet ) {
											cout<<"Type is not supported: "<<av[i+1]<<endl;
											continue;
									} else return 1;
							}
							
							ui.type = static_cast<cvq::udot_type>(typeval);
							ui.val = av[i+2];
							ret = vq->udot_add(atpos, av[i], "", ui);
							if(ret) {
									if(!quiet)
											cout<<vq->err_report()<<endl;
									else return(ret);
							} else {
									if(!quiet)
											cout<<"Dot file was changed, id.:"
												<<ui.id<<endl;
							}
					} else {
							if(!quiet) cout<<"invalid e-mail"<<endl;
							else return 1;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
