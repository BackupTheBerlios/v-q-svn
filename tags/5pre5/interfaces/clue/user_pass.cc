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
#include "error2str.hpp"
#include "cluemain.hpp"
#include "cdom_name2id.hpp"

#include <split.hpp>

using namespace std;
using namespace clue;

static const char *me = NULL;

/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-qs] [e-mail password ...]"<<endl
		<<"-s\tstdin mode - read e-mail addresses and passwords from stdin"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"Change password for mailbox. In stdin mode you should supply 2 lines"<<endl
		<<"for each mailbox: e-mail, password."<<endl;
}

/*
 *
 */
bool user_pass(const string &e, const string &p, 
		::vq::ivq_var & vq, bool quiet )
{
	static cdom_name2id dom_name2id;

	std::deque< std::string > esplit(text::split(e, "@"));
	if(!quiet) cout<<e<<": ";
	if( esplit.size() != 2 ) {
			if(!quiet)
					cout<<"invalid e-mail"<<endl;
			return quiet;
	}

	CORBA::String_var did;
	::vq::ivq::error_var ret(dom_name2id(vq, esplit.back(), did));
	if( ::vq::ivq::err_no != ret->ec ) {
			if( ! quiet )
					cout<<error2str(ret)<<endl;
			return quiet;
	}
	
	ret = vq->user_pass(did, esplit.front().c_str(), p.c_str());
	if( ::vq::ivq::err_no != ret->ec ) {
			if(!quiet)
					cout<<error2str(ret)<<endl;
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
int cluemain(int ac, char **av, cluemain_env & ce ) {
	me = *av;
	int opt;
	bool sin = false, quiet = false;
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
					if( user_pass(e, p, ce.vq, quiet) ) return 1;
			} while(cin);
	} else {
			if(quiet && ac > 0) ac = 1;
			for(int i=0; i < ac; i+=2 ) {
					if( user_pass(av[i], av[i+1], ce.vq, quiet) )
							return 1;
			}
	}
	return 0;
}
