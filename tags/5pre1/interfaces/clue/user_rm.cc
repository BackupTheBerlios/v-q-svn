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
	cerr<<"usage: "<<me<< " [-q] (e-mail){1,}"<<endl
		<<"\t-q\tquiet mode"<<endl
		<<endl
		<<"Delete users. It copies all files from users's mailbox to delete"<<endl
		<<"directory in home dir. So it's possible to restore user's messages."<<endl;
}

bool user_rm(const string &e)
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
	if((ret=vq->user_rm(u,d))) {
			if(!quiet)
					cout<<vq->err_report()<<endl;
			return quiet;
	} else {
			if(!quiet)
					cout<<"user was removed."<<endl;
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
			while( (opt=getopt(ac,av,"rqh")) != -1 ) {
					switch(opt) {
					case 'q':
							quiet = true;
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
			if( ! ac ) {
					usage();
					return(1);
			}

			vq = cvq::alloc();

			if(quiet) ac = 1;
			for(int i=0; i < ac; i++ ) {
					if( user_rm(av[i]) )
							return 1;
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
