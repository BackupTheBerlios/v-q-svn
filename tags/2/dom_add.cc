/*
Copyright (c) 2002 Pawel Niewiadomski
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
#include <errno.h>
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
    cerr<<"usage: "<<me<< " [-q] (domain){1,}"<<endl
        <<"\t-q\tquiet mode"<<endl;
}

/*
 *
 */
int main(int ac, char **av)
{
    me = *av;
    try {
            int opt;
            bool quiet=false;
            while( (opt=getopt(ac,av,"qh")) != -1 ) {
                    switch(opt) {
                    case 'q':
                            quiet=true;
                            break;
                    default:        
                    case '?':
                    case 'h':
                            usage();
                            return(1);
                    }
            }
            if( optind >= ac ) {
                    usage();
                    return(1);
            }
            ac -= optind;
            av += optind;

            cauth *auth;
            cvq *vq;
            vq_init(&vq, &auth);

            char ret;
            if( quiet ) ac = 1;
            for(int i=0; i < ac; i++ ) {
                    if(!quiet) cout<<av[i]<<": ";
                    if((ret=vq->dom_add(av[i]))) {
                            if(!quiet)
                                    cout<<vq->err_str(ret)<<": "<<vq->err()<<endl;
                            else return(ret);
                    } else {
                            if(!quiet)
                                    cout<<"Domain was added."<<endl;
                    }
            }
    } catch( const exception & e ) {
            cerr << "exception: " << e.what() << endl;
            return 1;
    }
    return 0;
}
