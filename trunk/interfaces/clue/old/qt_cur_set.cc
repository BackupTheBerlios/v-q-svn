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
	cerr<<"usage: "<<me<< " number (domain or e-mail){1,}"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"Change value of current mailbox's or domain usage."<<endl
		<<"Number is in bytes."<<endl;
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
			int32_t qc=0;
			bool quiet=false;
			while( (opt=getopt(ac,av,"h")) != -1 ) {
					switch(opt) {
					default:		
					case '?':
					case 'h':
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;
			if( ac < 2 ) {
					usage();
					return(1);
			}
			is.clear();
			is.str(av[0]);
			is>>qc;
			if(!is) {
					usage();
					return(2);
			}
			ac --;
			av ++;

			cvq *vq(cvq::alloc());

			uint8_t ret;
			char *atpos=NULL;
			if(quiet) ac = 1;
			for(int i=0; i < ac; i++ ) {
					if(!quiet) cout<<av[i]<<": ";
					atpos = rindex(av[i], '@');
					if(atpos) {
							*atpos = '\0';
							atpos++;
							ret = vq->qt_cur_set(atpos, av[i], qc);
					} else {
							ret = vq->qt_dom_cur_set(av[i], qc);
					}
					if(ret) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else return(ret);
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
