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
	cerr<<"usage: "<<me<< " (e-mail){1,}"<<endl
		<<endl
		<<"Print information about limits for given e-mail."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			while((opt=getopt(ac, av, "h")) != -1) {
					switch(opt) {
					default:
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;

			cvq::quota_value qb=0, qc=0;
			if( 1 > ac ) {
					usage();
					return(1);
			}

			cvq *vq(cvq::alloc());

			uint8_t ret;
			char *atpos=NULL;
			for(int i=0; i < ac; i++ ) {
					cout<<av[i]<<": ";
					atpos = rindex(av[i], '@');
					if(!atpos) {
							cout<<"Where's @?"<<endl;
							continue;
					}
					*atpos = '\0';
					atpos++;
					if( (ret = vq->qt_get(atpos, av[i], qb, qc)) ) {
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
