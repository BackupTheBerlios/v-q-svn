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

#include <vqmain.hpp>
#include <split.hpp>
#include <conf.hpp>

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace clue;

/*
 *
 */
void usage( const char *me, const std::string & uc_names )
{
	cerr<<"usage: "<<me<< " [options] (e-mail type value){1,}"<<endl
		<<"-q\tquiet mode, only first e-mail is processed"<<endl
		<<endl
		<<"Add options assiociated with mailboxes, for example autoresponse"<<endl
		<<"message, redirection address, etc. Type can be numeric or name."<<endl
		<<"In the second case it will be translated to numeric using file"<<endl
		<<uc_names<<'.'<<endl;
}

/*
 *
 */
int cluemain(int ac, char **av, ::vq::ivq_var & vq ) {
	std::string fn_uc_names(VQ_HOME+"/etc/ivq/user_conf_names");
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
					usage(*av, fn_uc_names);
					return(1);
			}
	}
	ac -= optind;
	av += optind;
	if( 1 > ac || ac % 3 ) {
			usage(*av, fn_uc_names);
			return(1);
	}

	if(quiet) ac=3;
			
	::vq::ivq::user_conf_info ui;
	std::deque< std::string > esplit;
	cdom_name2id dom_name2id;
	CORBA::String_var did;
	::vq::ivq::error_var ret;
	conf::cmapconf::map_type uc_names_map( conf::cmapconf(fn_uc_names).val_map() );
	conf::cmapconf::map_type::const_iterator name_itr;

	for(int i=0; i < ac && i+2 < ac; i+=3 ) {
			if(!quiet) cout<<av[i]<<": ";
			esplit = text::split(av[i], "@");
			if(esplit.size() != 2) {
					if(!quiet) 
							cout<<"invalid e-mail"<<endl;
					else 
							return 1;
			}
			
			ret = dom_name2id(vq, esplit.back(), did);
			if( ::vq::ivq::err_no != ret->ec ) {
					if( ! quiet )
							cout<<error2str(ret)<<endl;
					else
							return 1;
			}

			name_itr = uc_names_map.find(av[i+1]);
			try {
					ui.type = boost::lexical_cast<vq::ivq::user_conf_type>(
						name_itr == uc_names_map.end() 
						? name_itr->second : av[i+1]);
			} catch( boost::bad_lexical_cast & e ) {
					if( ! quiet ) {
							cout<<( name_itr == uc_names_map.end()
								? name_itr->second : av[i+1])<<": "<<e.what()<<endl;
							continue;
					} else 
							return 1;
			}

			ui.val = static_cast<const char *>(av[i+2]);
			ret = vq->user_conf_add( did, esplit.front().c_str(), "", ui);
			if( ::vq::ivq::err_no != ret->ec ) {
					if(!quiet)
							cout<<error2str(ret)<<endl;
					else 
							return ret->ec;
			} else {
					if(!quiet)
							cout<<"configuration added, identificator: "
								<<ui.id_conf<<endl;
			} 	
	}
	return 0;
}
