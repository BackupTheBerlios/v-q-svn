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
	cerr<<"usage: "<<me<< " (domain){1,}"<<endl
		<<"-s\tget limits copied to new domains"<<endl
		<<endl
		<<"Gets current limits for domains or global."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			bool system=false;
			while((opt=getopt(ac,av,"hs"))!=-1) {
					switch(opt) {
					case 's':
							system = true;
							break;
					default:
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;
			
			cvq::quota_value qb, qc;

			cvq *vq(cvq::alloc());
			uint8_t ret;
			if(system) {
					ret = vq->qt_global_def_get(qb, qc);
					if(ret) {
							cout<<vq->err_report()<<endl;
							return ret;
					} else 
							cout<<"Global default limits: "
								<<qb<<'S'<<','<<qc<<'C'<<endl;
			}
			
			if( 1 > ac ) {
					if( system ) return(0);
					usage();
					return(1);
			}
			for(int i=0; i < ac; i++ ) {
					cout<<av[i]<<": ";
					ret = vq->qt_def_get(av[i], qb, qc);
					if(ret) {
							cout<<vq->err_report()<<endl;
							return ret;
					} else {
							cout<<qb<<'S'<<','<<qc<<'C'<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
