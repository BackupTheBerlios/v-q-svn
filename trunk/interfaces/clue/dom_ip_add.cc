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
#include "cluemain.hpp"
#include "error2str.hpp"
#include "cdom_name2id.hpp"

using namespace std;
using namespace clue;

/*
 *
 */
void usage( const char * me )
{
	cerr<<"usage: "<<me<< " [-q] domain [IP ...]"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"Add IP addresses to a domain."<<endl;
}

/*
 *
 */
int cluemain( int ac, char **av, cluemain_env & ce ) {
	const char * me = *av;
	int opt;
	bool quiet=false;
	while( (opt=getopt(ac,av,"qh")) != -1 ) {
			switch(opt) {
			case 'q':
					quiet=true;
					break;
			default:		
			case '?':
			case 'h':
					usage( me );
					return(1);
			}
	}
	ac -= optind;
	av += optind;
	if( ac < 2 ) {
			usage( me );
			return(1);
	}

	static cdom_name2id dom_name2id;
	CORBA::String_var dom_id;
	::vq::ivq::error_var ret(dom_name2id(ce.vq, av[0], dom_id));
	
	if( ::vq::ivq::err_no != ret->ec ) {
			if( ! quiet )
					cout<<av[0]<<": "<<error2str(ret)<<endl;
			return 1;
	}

	if(quiet && ac > 0) ac = 2;
	for( int i=1;  i < ac; ++i ) {
			if(!quiet) cout<<av[i]<<": ";
			ret=ce.vq->dip_add(dom_id, av[i]);
			if( ::vq::ivq::err_no != ret->ec ) {
					if(!quiet)
							cout<<error2str(ret)<<endl;
					else 
							return 1;
			} else {
					if(!quiet)
							cout<<"IP was added."<<endl;
			}
	}
	return 0;
}
