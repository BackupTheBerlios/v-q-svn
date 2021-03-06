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
using namespace clue;

/*
 *
 */
void usage( const char * me )
{
	cerr<<"usage: "<<me<< " [-q] [domain ...]"<<endl
		<<"-q\tquiet mode"<<endl
		<<endl
		<<"List aliases for all domains or only those specified"<<endl;
}

/*
 *
 */
int cluemain( int ac, char **av, cluemain_env & ce ) {
	const char *me = *av;
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
	if( ! ac ) {
			usage( me );
			return(1);
	}

	::vq::ivq::error_var ret;
	if(quiet && ac > 0) ac = 1;

	::vq::ivq::domain_info_list_var dis;
	if( ac ) {
			::vq::ivq::id_type dom_id;
			::vq::ivq::domain_info di;
			dis = new ::vq::ivq::domain_info_list(ac);
			
			for( CORBA::ULong i=0;  i < ac; ++i ) {
					ret = ce.vq->dom_id( av[i], dom_id );
					if( ::vq::ivq::err_no != ret->ec ) {
							if( ! quiet )
									cout<<av[i]<<": "<<error2str(ret)<<endl;
							return 1;
					}
					dis->length(ac);
					dis[i].id_domain = dom_id;
					dis[i].domain = static_cast<const char *>(av[i]);
			}
	} else {
			ret = ce.vq->dom_ls(dis);
			if( ::vq::ivq::err_no != ret->ec ) {
					if( ! quiet )
							cout<<"dom_ls: "<<error2str(ret)<<endl;
					return 1;
			}
	}

	for( CORBA::ULong i=0, s=dis->length(); i<s; ++i ) {
			::vq::ivq::string_list_var alis;

			ret = ce.vq->da_ls_by_dom( dis[i].id_domain, alis );
			if( ::vq::ivq::err_no != ret->ec ) {
					if( ! quiet )
							cout<<av[i]<<": "<<error2str(ret)<<endl;
					else 
							return 1;
			}

			for( CORBA::ULong j=0, k=alis->length(); j<k; ++j ) {
					if(!quiet) 
							cout<<av[i]<<": ";
					cout<< alis[j] <<endl;
			}
	}
	return 0;
}
