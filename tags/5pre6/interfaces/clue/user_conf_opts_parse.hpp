#ifndef __USER_CONF_OPTS_PARSE_HPP
#define __USER_CONF_OPTS_PARSE_HPP

#include <conf.hpp>

#include <boost/lexical_cast.hpp>

#include <string>

typedef std::deque< vq::ivq::user_conf_type > types_array;

void usage( const char *, const std::string & );

int opts_parse( int & ac, char ** & av, const std::string &fn_uc_names,
		const conf::cmapconf::map_type & uc_names_map,
		types_array & types, bool & quiet ) {

	conf::cmapconf::map_type::const_iterator name_itr;
	types_array::value_type type;

	int opt;
	while( (opt=getopt(ac,av,"qht:")) != -1 ) {
			switch(opt) {
			case 'q':
					quiet = true;
					break;
			case 't':
					name_itr = uc_names_map.find(optarg);
					try {
							type = boost::lexical_cast<vq::ivq::user_conf_type>
									(name_itr == uc_names_map.end()
									 ? optarg : name_itr->second );
					} catch( boost::bad_lexical_cast & e ) {
							std::cerr<<"type "
								<<( name_itr == uc_names_map.end() 
									? optarg : name_itr->second )
								<<" is invalid"<<std::endl;
							return 1;
					}
					types.push_back(type);
			case '?':
			case 'h':
					usage(*av, fn_uc_names);
					return(1);
			}
	}
	ac -= optind;
	av += optind;
	return 0;
}

#endif // ifndef __USER_CONF_OPTS_PARSE_HPP
