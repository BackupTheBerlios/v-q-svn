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

#include <inttypes.h>
#include <exception>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "cfsvq.h"
#include "cdaemonauth.h"
#include "vq_init.h"

using namespace std;

static const char *me = NULL;
static bool quiet = false;
static cvq *vq = NULL;

/*
 *
 */
void usage()
{
    cerr<<"usage: "<<me<< " [-q] [-s] (e-mail password){1,}"<<endl
        <<"\t-s\tstdin mode - read e-mail addresses and passwords from stdin"<<endl
        <<"\t-l\tdon't check whether login is disallowed"<<endl
        <<"\t-q\tquiet mode"<<endl;
}

/*
 *
 */
bool user_add(const string &e, const string &p, uint8_t flags)
{
    string u, d;
    string::size_type atpos;
    
    if(!quiet) cout<<e<<": ";
    if( (atpos=e.find('@')) == string::npos ) {
            if(!quiet)
                    cout<<"E-mail doesn't contain @"<<endl;
            return quiet;
    }
    u = e.substr(0,atpos);
    d = e.substr(atpos+1);
    if( u.empty() || d.empty() ) {
            if(!quiet)
                    cout<<"E-mail is invalid"<<endl;
            return quiet;
    }
    char ret;
    if((ret=vq->user_add(u,d,p,flags))) {
            if(!quiet)
                    cout<<vq->err_str(ret)<<": "<<vq->err()<<endl;
            return quiet;
    } else {
            if(!quiet)
                    cout<<"User was added."<<endl;
    }
    return 0;
}
/*
 *
 */
int main(int ac, char **av)
{
    me = *av;
    try {
            int opt;
            bool sin = false;
            uint8_t flags=0;
            while( (opt=getopt(ac,av,"sqhl")) != -1 ) {
                    switch(opt) {
                    case 's':
                            sin = true;
                            break;
                    case 'q':
                            quiet=true;
                            break;
                    case 'l':
                            flags |= 1;
                            break;
                    default:        
                    case '?':
                    case 'h':
                            usage();
                            return(1);
                    }
            }
            if( ! sin && optind >= ac ) {
                    usage();
                    return(1);
            }
            ac -= optind;
            av += optind;
            if( ! sin && ac % 2 ) {
                    usage();
                    return(1);
            }

            cauth *auth;
            vq_init(&vq, &auth);
            
            if( sin ) {
                    string e,p;
                    do {
                            if( ! quiet ) {
                                    cout << "E-mail: ";
                                    cout.flush();
                            }
                            if( ! getline(cin,e) ) break;
                            if( ! quiet ) {
                                    cout << "Password: ";
                                    cout.flush();
                            }
                            if( ! getline(cin,p) ) break;
                            if( user_add(e,p,flags) ) return 1;
                    } while(cin);
            } else {
                    if(quiet) ac=1;
                    for(int i=0; i < ac; i+=2 ) {
                            if( user_add(av[i], av[i+1], flags) )
                                    return 1;
                    }
            }
    } catch( const exception & e ) {
            cerr << "exception: " << e.what() << endl;
            return 1;
    }
    return 0;
}
