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

static const char *me = NULL;
static bool quiet = false;
static cvq *vq = NULL;

/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-q] [-s] (e-mail password){1,}"<<endl
		<<"\t-s\tstdin mode - read e-mail addresses and passwords from stdin"<<endl
		<<"\t-q\tquiet mode"<<endl
		<<endl
		<<"Change password for mailbox. In stdin mode you should supply 2 lines"<<endl
		<<"for each mailbox: e-mail, password."<<endl;
}

/*
 *
 */
bool user_pass(const string &e, const string &p)
{
	string u, d;
	string::size_type atpos;
	if(!quiet) cout<<e<<": ";
	if( (atpos=e.find('@')) == string::npos
		|| (u = e.substr(0,atpos)).empty()
		|| (d = e.substr(atpos+1)).empty() ) {
			if(!quiet)
					cout<<"invalid e-mail"<<endl;
			return quiet;
	}
	char ret;
	if((ret=vq->user_pass(u,d,p))) {
			if(!quiet)
					cout<<vq->err_report()<<endl;
			return quiet;
	} else {
			if(!quiet)
					cout<<"password was changed."<<endl;
	}
	return 0;
}
/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			bool sin = false;
			while( (opt=getopt(ac,av,"sqh")) != -1 ) {
					switch(opt) {
					case 's':
							sin = true;
							break;
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
			if( ! sin && optind >= ac ) {
					usage();
					return(1);
			}
			ac -= optind;
			av += optind;
			if( ! sin && ac % 2 ) {
					usage();
					return(1);
			}

			vq = cvq::alloc();

			if( sin ) {
					string e,p;
					do {
							if( ! quiet ) {
									cout << "E-mail: ";
									cout.flush();
							}
							if( ! getline(cin,e) ) break;
							if( ! quiet ) {
									cout << "Password: ";
									cout.flush();
							}
							if( ! getline(cin,p) ) break;
							if( user_pass(e,p) ) return 1;
					} while(cin);
			} else {
					if(quiet) ac = 1;
					for(int i=0; i < ac; i+=2 ) {
							if( user_pass(av[i], av[i+1]) )
									return 1;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
