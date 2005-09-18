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

using namespace std;
using clue::error2str;
using clue::cdom_name2id;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-q] [domain ...]"<<endl
		<<"-q\tquiet mode (prints only ids.)"<<endl
		<<endl
		<<"Print domains' identificators."<<endl;
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

	if(quiet && ac > 0) ac=1;
	
	::vq::ivq::error_var ret;
	::vq::ivq::id_type dom_id;
	cdom_name2id dom_name2id;
	for(int i=0; i < ac; i++ ) {
			if(!quiet) cout<<av[i]<<": ";

			ret = dom_name2id(ce.vq, av[i], dom_id);
			if( ::vq::ivq::err_no != ret->ec ) {
					cout<<error2str(ret)<<endl;
			} else {
					cout<<dom_id<<endl;
			}
	}
	return 0;
}
