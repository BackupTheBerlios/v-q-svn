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

using namespace std;

static const char *me = NULL;

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

bool user_rm(const string &e, ::vq::ivq_var &vq, bool quiet ) {
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
	
	ret = vq->user_rm(did_itr->second, u.c_str());
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
int cluemain(int ac, char **av, ::vq::ivq_var & vq ) {
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
			if( user_rm(av[i], vq, quiet) )
					return 1;
	}
	return 0;
}
