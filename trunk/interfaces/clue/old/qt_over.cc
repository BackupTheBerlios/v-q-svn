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
	cerr<<"usage: "<<me<< " [options] (domain or e-mail){1,}"<<endl
		<<"-q\tquiet mode, check only first domain or e-mail, return 1 if over"<<endl
		<<endl
		<<"Check whether current space usage is larger than allowed."<<endl;
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
			while((opt=getopt(ac,av,"hq"))!=-1) {
						switch(opt) {
						case 'q':
								quiet=true;
								break;
						case 'h':
						case '?':
						default:
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
			if(quiet) ac = 1;
			for(int i=0; i < ac; i++ ) {
					atpos = rindex(av[i], '@');
					if(!quiet) cout<<av[i]<<": ";
					if(atpos) {
							*atpos = '\0';
							atpos++;
							ret = vq->qt_over(atpos, av[i]);
					} else {
							ret = vq->qt_dom_over(av[i]);
					}
					if(ret) {
							if(!quiet)
									cout<<"Over quota."<<endl;
							else return(ret);
					} else {
							if(!quiet)
									cout<<"Under quota."<<endl;
							else return(ret);
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
