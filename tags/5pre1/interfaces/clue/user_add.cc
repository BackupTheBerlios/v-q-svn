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

#include <inttypes.h>
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
		<<"\t-l\tdon't check whether login is disallowed"<<endl
		<<"\t-q\tquiet mode"<<endl
		<<endl
		<<"Program adds users. Default action is to read users and passwords"<<endl
		<<"from command line, check if login is allowed. In stdin mode"<<endl
		<<"programs input should be 2 lines for each mailbox: e-email, password."<<endl;
}

/*
 *
 */
bool user_add(const string &e, const string &p, uint8_t flags)
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
	if((ret=vq->user_add(u,d,p,flags))) {
			if(!quiet)
					cout<<vq->err_report()<<endl;
			return quiet;
	} else {
			if(!quiet)
					cout<<"user was added."<<endl;
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
			uint8_t flags=0;
			while( (opt=getopt(ac,av,"sqhl")) != -1 ) {
					switch(opt) {
					case 's':
							sin = true;
							break;
					case 'q':
							quiet=true;
							break;
					case 'l':
							flags |= 1;
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
							if( user_add(e,p,flags) ) return 1;
					} while(cin);
			} else {
					if(quiet) ac=1;
					for(int i=0; i < ac; i+=2 ) {
							if( user_add(av[i], av[i+1], flags) )
									return 1;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
