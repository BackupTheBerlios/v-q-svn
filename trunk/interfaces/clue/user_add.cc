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

#include <map>

using namespace std;

static const char *me = NULL;

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
bool user_add(const string &e, const string &p, uint8_t flags, 
		::vq::ivq_var & vq, bool quiet, bool eb_chk )
{
	typedef std::map< std::string, CORBA::String_var > domain2id_map;
	static domain2id_map dom2id;
	
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
	::vq::ivq::error_var ret;
	
	domain2id_map::const_iterator did_itr( dom2id.find(d) );
	if( dom2id.end() == did_itr ) {
			CORBA::String_var did;
			ret = vq->dom_id(d.c_str(), did);
			if( ::vq::ivq::err_no != ret->ec ) {
					if( ! quiet )
							cout<<error2str(ret)<<endl;
					return quiet;
			}
			did_itr = dom2id.insert( dom2id.begin(), std::make_pair(d, did) );
	}
	
	::vq::ivq::auth_info ai;
	ai.flags = flags;
	ai.id_domain = did_itr->second;
	ai.login = u.c_str();
	
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
int cluemain(int ac, char **av, ::vq::ivq_var & vq ) {
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
					if( user_add(e, p, flags, vq, quiet, eb_chk) ) return 1;
			} while(cin);
	} else {
			if(quiet) ac=1;
			for(int i=0; i < ac; i+=2 ) {
					if( user_add(av[i], av[i+1], flags, vq, quiet, eb_chk) )
							return 1;
			}
	}
	return 0;
}
