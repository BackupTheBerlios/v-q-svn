/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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

#include <exception>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <unistd.h>

#include "cvq.h"

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [options] (domain)*"<<endl
		<<"-q\tquiet mode"<<endl
		<<"-m number\tset default quota for users in given domain (default 0 - turn off)"<<endl
		<<"-s\tsystem, set default quota for new domains"<<endl
		<<endl
		<<"Changes space limit for domains or users that will be add."<<endl
		<<"It does not modify existing objects."<<endl;
}

/*
 *
 */
int main(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			istringstream is;
			cvq::quota_value qm=0;
			bool quiet=false, system=false;
			while( (opt=getopt(ac,av,"sqm:h")) != -1 ) {
					switch(opt) {
					case 'q':
							quiet=true;
							break;
					case 'm':
							is.clear();
							is.str(optarg);
							is>>qm;
							if(!is) {
									usage();
									return(2);
							}
							break;
					case 's':
							system = true;
							break;
					default:		
					case '?':
					case 'h':
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;

			cvq *vq(cvq::alloc());

			uint8_t ret;
			if(system) {
					ret = vq->qt_dom_def_set(qm);
					if(ret) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else return(ret);
					} else if(!quiet) {
							cout<<"Default domain's quota was set."<<endl;
					}
			}
			
			if( ! ac ) {
					if(system) return(0);
					usage();
					return(1);
			}
			if(quiet) ac = 1;
			for(int i=0; i < ac; i++ ) {
					if(!quiet) cout<<av[i]<<": ";
					ret = vq->qt_def_set(av[i], qm);
					if(ret) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else return(ret);
					} else {
							if(!quiet)
									cout<<"Default user's quota was set."<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
