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
#include "main.h"
#include "conf_home.h"

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>

using namespace std;
using namespace HOME_NS;

void usage(const char *me) {
	cerr<<"usage: "<<me<<" namespace name [multi] [class]"<<endl
		<<"reads value from stdin"<<endl;
}

int cppmain(int ac, char **av) {
	try {
			if(ac<3) {
					usage(*av);
					return 1;
			}
	
			string ln;
			getline(cin, ln);
			if( cin.bad() || ln.empty() ) {
					cerr<<"can't read input"<<endl;
					return 1;
			}
					 
			int val, mult=1;
			istringstream sval;
			
			sval.str(ln);
			if(ln[0] == '0') sval>>oct>>val;
			else sval>>val;

			if(!sval) {
					cerr<<ln<<" isn't a number"<<endl;
					return 4;
			}
			
			if( ac == 4 ) {
					sval.clear();
					sval.str(av[3]);
					sval>>mult;
					if(! sval) {
							cerr<<av[3]<<" isn't a number"<<endl;
							return 5;
					}
			}

			string ac_conf("auto/");
			ac_conf+=av[2];
			ac_conf+=".cc";
			
			ofstream out(ac_conf.c_str(), ios::trunc);
			out<<"// file autogenerated, don't edit!"<<endl
				<<"#include \"../cconf.h\""<<endl
				<<"namespace "<<av[1]<<" {"<<endl<<'\t';

			if(ac==5)
					out<<av[4];
			else 
					out<<"cintconf";

			out<<" ac_"<<av[2]<<"(\""<<conf_home
				<<"/etc/"<< av[1] <<'/'<<av[2]<<"\",\""
				<<val<<"\"";
			
			if( mult != 1 ) {
				out<<", "<<mult;
			}

			out<<");"<<endl
				<<"} // namespace "<<av[1]<<endl;
			out.close();

			if(! out) {
				cerr<<"can't write: "<<ac_conf<<": "<<strerror(errno)<<endl;
				return 1;
			}
	} catch( const exception & e ) {
			cerr<<"exception: "<<e.what()<<endl;
			return 1;
	}
	return 0;
}
