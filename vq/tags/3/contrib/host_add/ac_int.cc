#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>

#include "conf_home.h"

using namespace std;
using namespace host_add;

void usage(const char *me) {
	cerr<<"usage: "<<me<<" namespace name [multi] [class]"<<endl
		<<"reads value from stdin"<<endl;
}

int main(int ac, char **av) {
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
					val *= mult;
			}

			string ac_conf("auto/");
			ac_conf+=av[2];
			ac_conf+=".cc";
			
			ofstream out(ac_conf.c_str(), ios::trunc);
			out<<"// file autogenerated, don't edit!"<<endl
				<<"#include \"../cconf.h\""<<endl;

			if(*av[1] != '\0') out<<"namespace "<<av[1]<<" {"<<endl;
					
			if(ac==4) out<<av[4];
			else out<<"cintconf";

			out<<" ac_"<<av[2]<<"(\""<<conf_home
				<<"/etc/freemail/"<<av[2]<<"\",\""
				<<val<<"\");"<<endl;
			
			if(*av[1] != '\0') out<<"} // namespace "<<av[1]<<endl;
			
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
