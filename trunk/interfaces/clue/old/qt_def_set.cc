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
#include <sstream>
#include <cerrno>

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [options] quota (domain)*"<<endl
		<<"-q\tquiet mode"<<endl
		<<"-s\tsystem, set default quota for new domains"<<endl
		<<endl
		<<"Set default limits for new users. System wide quota is used"<<endl
		<<"for new domains. It sets default limits for new users."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av) {
	me = *av;
	try {
			int opt;
			cvq::quota_value qb, qc;
			bool quiet=false, system=false;
			while( (opt=getopt(ac,av,"sqh")) != -1 ) {
					switch(opt) {
					case 'q':
							quiet=true;
							break;
					case 's':
							system = true;
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

			cvq *vq(cvq::alloc());
			if( vq->qt_parse(av[0], qb, qc) ) {
					if( ! quiet ) 
							cout<<av[0]<<": "<<vq->err_report()<<endl;
					return 1;
			}

			uint8_t ret;
			if(system) {
					ret = vq->qt_global_def_set(qb, qc);
					if(ret) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else 
									return(ret);
					} else if(!quiet) {
							cout<<"Default limits were set."<<endl;
					}
			}
			
			if( ac == 1 ) {
					if(system) return(0);
					usage();
					return(1);
			}
			if(quiet) ac = 2;
			for(int i=1; i < ac; i++ ) {
					if(!quiet) cout<<av[i]<<": ";
					ret = vq->qt_def_set(av[i], qb, qc);
					if(ret) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else return(ret);
					} else {
							if(!quiet)
									cout<<"Default limits for new users were set."<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
