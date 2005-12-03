/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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

#include <boost/lexical_cast.hpp>

#include <map>

using namespace std;
using namespace clue;

static const char *me = NULL;

/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-q] [-s] [-f flags] [-d home] [e-mail ...]"<<endl
		<<"-s\tstdin mode - read e-mail addresses from stdin"<<endl
		<<"-d\tset home directory for user"<<endl
		<<"-q\tquiet mode"<<endl
		<<"-f\tset flags for accounts"<<endl
		<<endl
		<<"Program modifies users' accounts. Default action is to read users "<<endl
		<<"from command line. In stdin mode"<<endl
		<<"programs input should be 1 line for each mailbox: e-email."<<endl;
}

/*
 *
 */
bool user_rep(const string &e, ::vq::ivq::uif_type flags, 
		::vq::ivq_var & vq, bool quiet, const string &home, bool home_chg )
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
	ai.dir = home.c_str();
	
	::vq::ivq::error_var ret = dom_name2id(vq, esplit.back(), ai.id_domain);
	if( ::vq::ivq::err_no != ret->ec ) {
			if( ! quiet )
					cout<<error2str(ret)<<endl;
			return quiet;
	}

	ret = vq->user_rep( ai, 0, home_chg ? 1 : 0 );
	if( ::vq::ivq::err_no != ret->ec ) {
			if(!quiet)
					cout<<error2str(ret)<<endl;
			return quiet;
	} else {
			if(!quiet)
					cout<<"user was modified."<<endl;
	}
	return 0;
}

/*
 *
 */
int cluemain(int ac, char **av, cluemain_env & ce ) {
	me = *av;
	bool quiet = false;
	int opt;
	bool sin = false;
	std::string home;
	bool home_chg = false;
	::vq::ivq::uif_type flags=0;
	while( (opt=getopt(ac,av,"f:sqhld:")) != -1 ) {
			switch(opt) {
			case 's':
					sin = true;
					break;
			case 'q':
					quiet=true;
					break;
			case 'd':
					home = optarg;
					home_chg = true;
					break;
			case 'f':
					flags = boost::lexical_cast< ::vq::ivq::uif_type >(optarg);
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
	if( ! sin ) {
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
					if( user_rep(e, flags, ce.vq, quiet, home, home_chg) ) 
							return 1;
			} while(cin);
	} else {
			if(quiet && ac > 0) ac=1;
			for(int i=0; i < ac; ++i ) {
					if( user_rep(av[i], flags, ce.vq, quiet, home, home_chg) )
							return 1;
			}
	}
	return 0;
}
