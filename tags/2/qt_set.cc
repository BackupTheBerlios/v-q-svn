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
#include <sstream>
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
    cerr<<"usage: "<<me<< " [options] (domain or e-mail){1,}"<<endl
        <<"-q\tquiet mode"<<endl
        <<"-m number\tset max quota (default 0 - turn off)"<<endl
        <<"-c number\tset current usage (must specify -m option before)"<<endl;
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
            cfsvq::quota_value qm=0, qc=0;
            bool qmwas=false, quiet=false, qcwas=false;
            while( (opt=getopt(ac,av,"qm:c:h")) != -1 ) {
                    switch(opt) {
                    case 'q':
                            quiet=true;
                            break;
                    case 'c':
                            is.clear();
                            is.str(optarg);
                            is>>qc;
                            if(!is) {
                                    usage();
                                    return(2);
                            }
                            qcwas = true;
                            break;
                    case 'm':
                            is.clear();
                            is.str(optarg);
                            is>>qm;
                            if(!is) {
                                    usage();
                                    return(2);
                            }
                            qmwas = true;
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
            if( 1 > ac || (qcwas && !qmwas) || !qmwas) {
                    usage();
                    return(1);
            }

            cauth *auth;
            cvq *vq;
            vq_init(&vq, &auth);

            uint8_t ret;
            char *atpos=NULL;
            if(quiet) ac=1;
            for(int i=0; i < ac; i++ ) {
                    atpos = rindex(av[i], '@');
                    if(!quiet) cout<<av[i]<<": ";
                    if(atpos) {
                            *atpos = '\0';
                            atpos++;
                            if( qcwas ) {
                                    ret = vq->qt_set(atpos, av[i], qm, qc);
                            } else {
                                    ret = vq->qt_set(atpos, av[i], qm );
                            }
                    } else {
                            if( qcwas ) {
                                    ret = vq->qt_dom_set(av[i], qm, qc);
                            } else {
                                    ret = vq->qt_dom_set(av[i], qm);
                            }
                    }
                    if(ret) {
                            if(!quiet)
                                    cout<<vq->err_str(ret)<<": "<<vq->err()<<endl;
                            else return(ret);
                    } else {
                            if(!quiet)
                                    cout<<"Quota was set."<<endl;
                    }
            }
    } catch( const exception & e ) {
            cerr << "exception: " << e.what() << endl;
            return 1;
    }
    return 0;
}
