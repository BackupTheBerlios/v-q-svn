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
#include "user_conf_opts_parse.hpp"

#include <split.hpp>
#include <vqmain.hpp>

using namespace std;
using namespace clue;

/*
 *
 */
void usage(const char * me, const std::string & uc_names)
{
	cerr<<"usage: "<<me<< " [-q] [-t type] [e-mail [entry's_id] ...]"<<endl
		<<"-q\tquiet mode, only first e-mail is processed"<<endl
		<<"-t\tremove only this type of configuration, entry's id. must be ommited"<<endl
		<<endl
		<<"Delete configuration option assiociated with user's mailbox."<<endl
		<<"Warning: some options must be present for valid processing of"<<endl
		<<"delivieries, etc. Use at your own risk. Type can be literal if "<<endl
		<<uc_names<<" includes mapping to numeric."<<endl;
}

/*
 *
 */
int cluemain(int ac, char **av, cluemain_env & ce )
{
	const char * me = *av;
	std::string fn_uc_names(VQ_HOME+"/etc/ivq/user_conf_names");
	conf::cmapconf::map_type uc_names_map( conf::cmapconf(fn_uc_names).val_map() );
	types_array types;

	bool quiet=false;
	int ar = opts_parse(ac, av, fn_uc_names, uc_names_map, types, quiet);
	if( ar ) return ar;

	if( 1 > ac || (types.empty() && ac % 2) ) {
			usage(me, fn_uc_names);
			return 1;
	}

	CORBA::String_var did;
	cdom_name2id dom_name2id;
	vq::ivq::error_var ret;
	vq::ivq::user_conf_info_list_var ucis;
	std::deque< std::string > esplit;
	types_array::const_iterator type_itr, type_end;
	
	if(quiet && ac > 0) ac=2;
	for(int i=0; i < ac && i+1 < ac; i++ ) {
			esplit = text::split(av[i], "@");
			if( esplit.size() != 2 ) {
					if( ! quiet ) {
							cout<<av[i]<<": invalid e-mail"<<endl;
							continue;
					} else return 1;
			}
			ret = dom_name2id(ce.vq, esplit.back().c_str(), did);
			if( vq::ivq::err_no != ret->ec ) {
					if( ! quiet ) {
							cout<<av[i]<<": "<<error2str(ret)<<endl;
							continue;
					} else return 1;
			}

			if( types.empty() ) {
					ret = ce.vq->user_conf_rm(did, esplit.front().c_str(), "", 
						static_cast<const char *>(av[i+1]) );
					if( vq::ivq::err_no != ret->ec ) {
							if( ! quiet ) {
									cout<<av[i]<<": "<<error2str(ret)<<endl;
									continue;
							} else return 1;
					}
					cout<<av[i]<<": removed: "<<av[i+1]<<endl;
			} else {
					for( type_itr=types.begin(), type_end=types.end();
								type_itr != type_end; ++type_itr ) {

							ret = ce.vq->user_conf_rm_by_type(did, 
								esplit.front().c_str(), "", *type_itr );
							if( vq::ivq::err_no != ret->ec ) {
									if( ! quiet ) {
											cout<<av[i]<<": "<<error2str(ret);
											continue;
									} else return 1;
							}
							cout<<av[i]<<": removed type: "<<*type_itr<<endl;
					}
			}
	}

	return 0;
}
