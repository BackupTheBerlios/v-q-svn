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
#include "cluemain.hpp"
#include "error2str.hpp"
#include "cdom_name2id.hpp"

#include <split.hpp>

#include <map>

using namespace std;
using namespace clue;

static const char *me = NULL;

/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-q] [-s] [e-mail password ...]"<<endl
		<<"-s\tstdin mode - read e-mail addresses and passwords from stdin"<<endl
		<<"-l\tdon't check whether login is disallowed"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"Program adds users. Default action is to read users and passwords"<<endl
		<<"from command line, check if login is allowed. In stdin mode"<<endl
		<<"programs input should be 2 lines for each mailbox: e-email, password."<<endl;
}

/*
 *
 */
bool user_add(const string &e, const string &p, uint8_t flags, 
		::vq::ivq_var & vq, bool quiet, bool eb_chk )
{
	static cdom_name2id dom_name2id;

	if(!quiet) cout<<e<<": ";

	std::deque<std::string> esplit( text::split(e, "@") );
	if( esplit.size() != 2 ) {
			if(!quiet)
					cout<<"invalid e-mail"<<endl;
			return quiet;
	}
	::vq::ivq::user_info ai;
	ai.flags = flags;
	ai.login = esplit.front().c_str();
	
	::vq::ivq::error_var ret(dom_name2id(vq, esplit.back(), ai.id_domain));
	if( ::vq::ivq::err_no != ret->ec ) {
			if( ! quiet )
					cout<<error2str(ret)<<endl;
			return quiet;
	}

	ret = vq->user_add(ai, eb_chk ? TRUE : FALSE );
	if( ::vq::ivq::err_no != ret->ec ) {
			if(!quiet)
					cout<<error2str(ret)<<endl;
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
int cluemain(int ac, char **av, cluemain_env & ce ) {
	me = *av;
	bool quiet = false, eb_chk = true;
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
					eb_chk = false;
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
					if( user_add(e, p, flags, ce.vq, quiet, eb_chk) ) return 1;
			} while(cin);
	} else {
			if(quiet) ac=1;
			for(int i=0; i < ac; i+=2 ) {
					if( user_add(av[i], av[i+1], flags, ce.vq, quiet, eb_chk) )
							return 1;
			}
	}
	return 0;
}
