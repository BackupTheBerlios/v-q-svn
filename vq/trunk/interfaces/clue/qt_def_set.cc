/*
Copyright (c) 2002,2003,2004 Pawel Niewiadomski
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
#include "cvq.h"
#include "main.h"

#include <unistd.h>

#include <exception>
#include <iostream>
#include <sstream>
#include <cerrno>

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [options] quota (domain)*"<<endl
		<<"-q\tquiet mode"<<endl
		<<"-s\tsystem, set default quota for new domains"<<endl
		<<endl
		<<"Set default limits for new users. System wide quota is used"<<endl
		<<"for new domains. It sets default limits for new users."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av) {
	me = *av;
	try {
			int opt;
			cvq::quota_value qb, qc;
			bool quiet=false, system=false;
			while( (opt=getopt(ac,av,"sqh")) != -1 ) {
					switch(opt) {
					case 'q':
							quiet=true;
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
			if( vq->qt_parse(av[0], qb, qc) ) {
					if( ! quiet ) 
							cout<<av[0]<<": "<<vq->err_report()<<endl;
					return 1;
			}

			uint8_t ret;
			if(system) {
					ret = vq->qt_global_def_set(qb, qc);
					if(ret) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else 
									return(ret);
					} else if(!quiet) {
							cout<<"Default limits were set."<<endl;
					}
			}
			
			if( ac == 1 ) {
					if(system) return(0);
					usage();
					return(1);
			}
			if(quiet) ac = 2;
			for(int i=1; i < ac; i++ ) {
					if(!quiet) cout<<av[i]<<": ";
					ret = vq->qt_def_set(av[i], qb, qc);
					if(ret) {
							if(!quiet)
									cout<<vq->err_report()<<endl;
							else return(ret);
					} else {
							if(!quiet)
									cout<<"Default limits for new users were set."<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
