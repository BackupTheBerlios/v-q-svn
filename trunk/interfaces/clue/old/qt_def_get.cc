/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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
	cerr<<"usage: "<<me<< " (domain){1,}"<<endl
		<<"-s\tget limits copied to new domains"<<endl
		<<endl
		<<"Gets current limits for domains or global."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			bool system=false;
			while((opt=getopt(ac,av,"hs"))!=-1) {
					switch(opt) {
					case 's':
							system = true;
							break;
					default:
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;
			
			cvq::quota_value qb, qc;

			cvq *vq(cvq::alloc());
			uint8_t ret;
			if(system) {
					ret = vq->qt_global_def_get(qb, qc);
					if(ret) {
							cout<<vq->err_report()<<endl;
							return ret;
					} else 
							cout<<"Global default limits: "
								<<qb<<'S'<<','<<qc<<'C'<<endl;
			}
			
			if( 1 > ac ) {
					if( system ) return(0);
					usage();
					return(1);
			}
			for(int i=0; i < ac; i++ ) {
					cout<<av[i]<<": ";
					ret = vq->qt_def_get(av[i], qb, qc);
					if(ret) {
							cout<<vq->err_report()<<endl;
							return ret;
					} else {
							cout<<qb<<'S'<<','<<qc<<'C'<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
