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

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " (e-mail){1,}"<<endl
		<<endl
		<<"List options assiociated with specified mailboxes."<<endl
		<<"Some of them may be in unreadable form that must be processed"<<endl
		<<"by some utilities. It prints: address, entry's id. number, type,"<<endl
		<<"value."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			while( (opt=getopt(ac,av,"h")) != -1 ) {
					switch(opt) {
					case '?':
					case 'h':
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;
			if( 1 > ac ) {
					usage();
					return(1);
			}

			cvq *vq(cvq::alloc());

			uint8_t ret;
			char *atpos=NULL;
			vector<cvq::udot_info>::size_type size, j;
			vector<cvq::udot_info> uis;
			for(int i=0; i < ac; i++ ) {
					atpos = rindex(av[i], '@');
					if(atpos) {
							*atpos = '\0';
							atpos++;
							ret = vq->udot_ls(atpos, av[i], "", uis);
							if(ret) {
									cout<<av[i]<<'@'<<atpos<<": "
										<<vq->err_report()<<endl;
									continue;
							}
							for( j=0, size=uis.size(); j<size; ++j ) {
								cout<<av[i]<<'@'<<atpos<<": "
									<<uis[j].id<<": "
									<<uis[j].type<<": "
									<<uis[j].val<<endl;
							}
					} else {
							cout<<av[i]<<": invalid e-mail"<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
