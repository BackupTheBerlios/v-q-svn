/*
Copyright (c) 2003,2004 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
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