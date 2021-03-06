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
#include <string>
#include <cerrno>

using namespace std;
using namespace HOME_NS;

void usage(const char *me) {
	cerr<<"usage: "<<me<<" namespace name [del_lf]"<<endl;
}

int cppmain(int ac, char **av) {
	try {
			if(ac<3) {
					usage(*av);
					return 1;
			}
		
			string file;
			string ln;
			while(getline(cin, ln)) {
					file += ln;
					if( ac != 4 ) file += '\n';
			}

			if( file.empty() ) {
					cerr<<"string is empty"<<endl;
					return 1;
			}

			string ac_conf("auto/");
			ac_conf+=av[2];
			ac_conf+=".cc";
			
			ofstream out(ac_conf.c_str(), ios::trunc);
			out<<"// file autogenerated, don't edit!"<<endl
				<<"#include \"../cconf.h\""<<endl
				<<"namespace "<<av[1]<<" {"<<endl
				<<"\tclnconf ac_"<<av[2]<<"(\""<<conf_home
				<<"/etc/"<< av[1] <<'/'<<av[2]<<"\",\"";

			string::const_iterator iter, end=file.end();
			for( iter=file.begin(); iter!=end; ++iter ) {
					out<< "\\x" << hex << (int) *iter;
			}

			out<<"\");"<<endl
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
