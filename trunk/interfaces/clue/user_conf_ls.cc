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
#include <conf.hpp>
#include <vqmain.hpp>

using namespace std;
using namespace clue;

/*
 *
 */
void usage( const char *me, const std::string & uc_names )
{
	cerr<<"usage: "<<me<< " (e-mail){1,}"<<endl
		<<endl
		<<"List options assiociated with specified mailboxes."<<endl
		<<"Some of them may be in unreadable form that must be processed"<<endl
		<<"by some utilities. It prints: address, entry's identificator, type,"<<endl
		<<"value. It also print user friendly type's name if it can find "<<endl
		<<"mapping in "<<uc_names<<'.'<<endl;
}

/*
 *
 */
int cluemain(int ac, char **av, vq::ivq_var & vq )
{
	int opt;
	std::string fn_uc_names(VQ_HOME+"/etc/ivq/user_conf_names");
	while( (opt=getopt(ac,av,"h")) != -1 ) {
			switch(opt) {
			case '?':
			case 'h':
					usage(*av, fn_uc_names);
					return(1);
			}
	}
	ac -= optind;
	av += optind;
	if( 1 > ac ) {
			usage(*av, fn_uc_names);
			return(1);
	}

	CORBA::String_var did;
	cdom_name2id dom_name2id;
	vq::ivq::error_var ret;
	vq::ivq::user_conf_info_list_var ucis;
	conf::cmapconf::map_type uc_names_map( conf::cmapconf(fn_uc_names).val_map() );
	conf::cmapconf::map_type::const_iterator name_itr;
	std::deque< std::string > esplit;
	
	for(int i=0; i < ac; i++ ) {
			esplit = text::split(av[i], "@");
			cout<<av[i]<<": ";
			if( esplit.size() != 2 ) {
					cout<<"invalid e-mail"<<endl;
					continue;
			}
			ret = dom_name2id(vq, esplit.back().c_str(), did);
			if( vq::ivq::err_no != ret->ec ) {
					cout<<error2str(ret)<<endl;
					continue;
			}
			ret = vq->user_conf_ls(did, esplit.front().c_str(), "", ucis);
			if( vq::ivq::err_no != ret->ec ) {
					cout<<error2str(ret)<<endl;
					continue;
			}
			for( CORBA::ULong j = 0, size=ucis->length(); j<size; ++j ) {
					cout<<ucis[j].id_conf;
					name_itr = uc_names_map.find(
						static_cast<const char *>(ucis[j].id_conf));
					if( name_itr != uc_names_map.end() )
							cout<<" ("<<name_itr->second<<')';
					
					cout<<": "<<ucis[j].type<<": "<<ucis[j].val<<endl;
			}
	}
	return 0;
}
