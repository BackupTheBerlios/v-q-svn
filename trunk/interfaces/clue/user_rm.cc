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
	cerr<<"usage: "<<me<< " [-q] [e-mail ...]"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"Delete users. It copies all files from users's mailbox to delete"<<endl
		<<"directory in home dir. So it's possible to restore user's messages."<<endl;
}

bool user_rm(const string &e, ::vq::ivq_var &vq, bool quiet ) {
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
	
	ret = vq->user_rm(did, esplit.front().c_str());
	if( ::vq::ivq::err_no != ret->ec ) {
			if(!quiet)
					cout<<error2str(ret)<<endl;
			return quiet;
	} else {
			if(!quiet)
					cout<<"user was removed"<<endl;
	}
	return 0;
}
/*
 *
 */
int cluemain(int ac, char **av, cluemain_env & ce ) {
	me = *av;
	int opt;
	bool quiet;
	while( (opt=getopt(ac,av,"qh")) != -1 ) {
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

	if(quiet) ac = 1;
	for(int i=0; i < ac; i++ ) {
			if( user_rm(av[i], ce.vq, quiet) )
					return 1;
	}
	return 0;
}
