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

using namespace std;
using clue::error2str;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-q] [domain ...]"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"Add domains in mail system. It does not add any users, so"<<endl
		<<"after creating domain you should add at least postmaster user"<<endl
		<<"(because it's required and any host has to have it)"<<endl;
}

/*
 *
 */
int cluemain(int ac, char **av, cluemain_env & ce ) {
	me = *av;
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
					usage();
					return(1);
			}
	}
	if( optind >= ac ) {
			usage();
			return(1);
	}
	ac -= optind;
	av += optind;

	if( quiet && ac > 0 ) ac = 1;

	CORBA::String_var dom_id;
	::vq::ivq::error_var ret;
	for(int i=0; i < ac; i++ ) {
			if(!quiet) cout<<av[i]<<": ";
			ret=ce.vq->dom_add(av[i], dom_id);
			if( ::vq::ivq::err_no != ret->ec ) {
					if(!quiet)
							cout<<error2str(ret)<<endl;
					else return ret->ec;
			} else {
					if(!quiet)
							cout<<"Domain was added."<<endl;
			}
	}
	return 0;
}
