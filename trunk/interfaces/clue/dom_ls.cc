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

using namespace std;
using clue::error2str;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<<endl
		<<endl
		<<"List all domains registered."<<endl;
}

/*
 *
 */ 
int cluemain(int ac, char **av, cluemain_env & ce ) {
	me = *av;
	int opt;
	while( (opt=getopt(ac,av,"h")) != -1 ) {
			switch(opt) {
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

	::vq::ivq::domain_info_list_var dis;
	::vq::ivq::error_var ret(ce.vq->dom_ls(dis));
	if( ::vq::ivq::err_no != ret->ec ) {
			cout<<"dom_ls: "<<error2str(ret)<<endl;
	}
	
	for(CORBA::ULong i=0, s=dis->length(); i < s; ++i ) {
			cout<<dis[i].id_domain<<' '<<dis[i].domain<<endl;
	}
	return 0;
}
