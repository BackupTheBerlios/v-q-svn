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

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-q] domain [ip ...]"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"Assign IP addresses to domain. Usually user_auth if"<<endl
		<<"user's name is not an e-mail address tries to guess name of"<<endl
		<<"domain in which user should be searched. It does it from environment"<<endl
		<<"variables set by tcpserver. Alternatively you it can use database"<<endl
		<<"created by this program"<<endl;
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
			if( 1 >= ac ) {
					usage();
					return(1);
			}

			cvq *vq(cvq::alloc());

			char ret;
			if(quiet && ac > 0) ac=2;
			for(int i=1; i < ac; i++ ) {
					if(!quiet) cout<<av[i]<<": ";
					if((ret=vq->dom_ip_add(av[0],av[i]))) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else return(ret);
					} else {
							if(!quiet)
									cout<<"Domain IP was added."<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
