/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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

#include <exception>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <unistd.h>

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [options] quota (e-mail){1,}"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"Quota is represented by numbers separated by coma, example string:"<<endl
		<<"1000C,600000S means limit number of messages to 1000, and total size"<<endl
		<<"to 600000 bytes."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			istringstream is;
			cvq::quota_value qb=0, qc=0;
			bool quiet=false;

			while( (opt=getopt(ac,av,"qh")) != -1 ) {
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
			if( 2 > ac) {
					usage();
					return(1);
			}

			cvq *vq(cvq::alloc());
			if( vq->qt_parse(av[0], qb, qc) ) {
					cout<<av[0]<<": "<<vq->err_report()<<endl;
					return 1;
			}

			uint8_t ret;
			char *atpos=NULL;
			if(quiet) ac=2;
			for(int i=1; i < ac; i++ ) {
					if(!quiet) cout<<av[i]<<": ";

					atpos = rindex(av[i], '@');
					if(!atpos) {
							if(!quiet) {
									cout<<"Where's @?"<<endl;
									continue;
							}
							return 1;
					}
					*atpos = '\0';
					atpos++;

					if( (ret = vq->qt_set(atpos, av[i], qb, qc)) ) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else 
									return(ret);
					} else {
							if(!quiet)
									cout<<"Quota was set."<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
