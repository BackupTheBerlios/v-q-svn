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

#include "cfsvq.h"
#include "cdaemonauth.h"
#include "vq_init.h"

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " (domain or e-mail){1,}"<<endl
		<<endl
		<<"Print information about space used and limit set."<<endl;
}

/*
 *
 */
int main(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			while((opt=getopt(ac, av, "h")) != -1) {
					switch(opt) {
					default:
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;

			cfsvq::quota_value qm=0, qc=0;
			if( 1 > ac ) {
					usage();
					return(1);
			}

			cauth *auth;
			cvq *vq;
			vq_init(&vq, &auth);

			uint8_t ret;
			char *atpos=NULL;
			for(int i=0; i < ac; i++ ) {
					cout<<av[i]<<": ";
					atpos = rindex(av[i], '@');
					if(atpos) {
							*atpos = '\0';
							atpos++;
							ret = vq->qt_get(atpos, av[i], qm, qc);
					} else {
							ret = vq->qt_dom_get(av[i], qm, qc);
					}
					if(ret) {
							cout<<vq->err_str(ret)<<": "<<vq->err()<<endl;
					} else {
							cout<<"Quota: "<<qc<<'/'<<qm<<endl;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
