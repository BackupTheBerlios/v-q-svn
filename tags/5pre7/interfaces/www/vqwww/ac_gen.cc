#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>

#include "conf_home.h"

using namespace std;
using namespace HOME_NS;

void usage(const char *me) {
	cerr<<"usage: "<<me<<" namespace type name"<<endl;
}

int main(int ac, char **av) {
	try {
			if(ac!=4) {
					usage(*av);
					return 1;
			}
		
			string ac_conf("auto/");
			ac_conf+=av[3];
			ac_conf+=".cc";
			
			ofstream out(ac_conf.c_str(), ios::trunc);
			out<<"// file autogenerated, don't edit!"<<endl
				<<"#include \"../cconf.h\""<<endl
				<<"namespace "<<av[1]<<" {"<<endl
				<<"\t"<<av[2]<<" ac_"<<av[3]<<"(\""<<conf_home
				<<"/etc/vqwww/"<<av[3]<<"\");"<<endl
				<<"}"<<endl;

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