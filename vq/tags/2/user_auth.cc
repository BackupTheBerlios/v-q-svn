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

#include <sys/types.h>
#include <exception>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <sstream>

#include "conf_vq_uid.h"
#include "conf_vq_gid.h"
#include "cdaemonauth.h"
#include "getline.h"
#include "conf_atchars.h"
#include "lower.h"
#include "hmac_md5.h"
#include "lmd5.h"
#include "ccurlog.h"
#include "vq_init.h"
#include "sig.h"
#include "sys.h"

using namespace std;

char *me = NULL, **av;
int ac;
const char *remip = NULL, *locip = NULL, *defdom = NULL;
bool quiet = false;
cvq *vq = NULL;
string resp, pass;
enum { smtp, pop3 } contype;
cvq::auth_info vqai;
clogger *olog = NULL;

/*
 *
 */
char read_auth_info()
{
    if( ! getline(3,vqai.user,'\0')
        || ! getline(3,pass,'\0')
        || ! getline(3,resp,'\0')) {
            cerr << me<<": can't read authorization info: "<<strerror(errno)<<endl;
            return(1);
    }
    vqai.user = lower(vqai.user);
    if( conf_atchars.empty() ) {
            cerr << me<<": conf_atchars empty"<<endl;
            return(1);
    }
    string::size_type atpos = vqai.user.find_last_of(conf_atchars);
    if( atpos != vqai.user.npos ) {
            vqai.dom = vqai.user.substr(atpos+1);
            vqai.user = vqai.user.substr(0,atpos);
    }
    if( (!vqai.dom.empty() && !vq->dom_val(vqai.dom)) 
       || vqai.user.empty() || !vq->user_val(vqai.user) ) {
            olog->ae("username or domain include disallowed chars",
                    vqai.dom, vqai.user );
            return(1);
    }
    if( vqai.dom.empty() ) vqai.dom = defdom ? defdom : locip;
    if( pass.empty() ) {
            olog->ae("empty password", vqai.dom, vqai.user );
            return(1);
    }
    return 0;
}

/*
 *
 */
char read_env()
{
    const char *tmp;
    int itmp;
    while((itmp=getopt(ac, av, "D:")) != -1) {
            switch(itmp) {
            case 'D':
                    defdom = optarg;
                    break;
            }
    }
    ac -= optind;
    av += optind;

    remip = getenv("TCPREMOTEIP");
    if(!remip) remip = "";
    
    locip = getenv("TCPLOCALHOST");
    if( !locip || *locip == '\0' ) {
            locip = getenv("TCPLOCALIP");
            if(!locip || *locip == '\0') {
                    cerr<<me<<": TCPLOCALIP is not set"<<endl;
                    return(1);
            }
    }            
    if( !(tmp=getenv("TCPLOCALPORT")) ) {
            cerr<<me<<": TCPLOCALPORT is not set"<<endl;
            return(1);
    } else {
            istringstream is;
            is.str(tmp);
            is >> itmp;
            if( ! is || itmp<0 || itmp>0xffff ) {
                    cerr<<me<<": TCPLOCALPORT is not a valid port number"<<endl;
                    return(1);
            }
    }
    switch(itmp) {
    default:
    case 995:
    case 110:
            if(ac<1) {
                    cerr<<me<<": no command was given"<<endl;
                    return(1);
            }
            contype = pop3;
            break;
    case 465:
    case 25:
    case 250: // ESMTP
            contype = smtp;
            break;
    }
    return 0;
}

/*
 * return 0 if apop authorization is successful
 */
char auth_apop()
{
    MD5_CTX ctx;
    unsigned char digest[16];
    string hexdigest;
    MD5Init(&ctx);
    MD5Update(&ctx,(const unsigned char*)resp.data(),resp.length());
    MD5Update(&ctx,(const unsigned char*)vqai.pass.data(),vqai.pass.length());
    MD5Final(digest,&ctx);
    hexdigest = to_hex(digest,16);
    return pass==hexdigest ? 0 : 1;
}
/*
 * return 0 if cram-md5 authorization is successful
 */
char auth_cram()
{
    unsigned char digest[16];
    string hexdigest;
    hmac_md5((const unsigned char*)vqai.pass.data(),
                    vqai.pass.length(),(const unsigned char*)resp.data(),
                    resp.length(),digest);
    hexdigest = to_hex(digest,16);
    return pass==hexdigest ? 0 : 1;
}

/*
 *
 */
char login_smtp()
{
    olog->an("SMTP login successful", vqai.dom, vqai.user );
    return(0);
}

/*
 *
 */
char login_pop()
{
#ifdef DEBUG
    cerr<<"login_pop:1"<<endl;
#endif
    if( setenv("HOME", vqai.dir.c_str(), 1) ) {
//            cerr<<me<<": setenv: "<<strerror(errno)<<endl;
            return(1);
    }
#ifdef DEBUG
    cerr<<"login_pop:2"<<endl;
#endif
    if( chdir(vqai.dir.c_str()) ) {
//            cerr<<me<<": chdir: "<<vqai.dir<<": "<<strerror(errno)<<endl;
            return(1);
    }
#ifdef DEBUG
    cerr<<"login_pop:3"<<endl;
#endif
    if( setgid(conf_vq_gid_i) || setegid(getgid()) 
        || setuid(conf_vq_uid_i) || seteuid(getuid()) ) {
//            cerr<<me<<": can't change gid/uid: "<<strerror(errno)<<endl;
            return(1);
    }
#ifdef DEBUG
    cerr<<"login_pop:4"<<endl;
#endif
    olog->an("POP3 login successful", vqai.dom, vqai.user );
#ifdef DEBUG
    cerr<<"login_pop:6"<<endl;
#endif
    if( olog ) delete olog;
#ifdef DEBUG
    cerr<<"login_pop:5"<<endl;
#endif
    execvp(*av,av);
//    cerr<<me<<": exec: "<<strerror(errno)<<endl;
    return(1);
} 

/*
 *
 */
char proc_auth_info()
{
#ifdef DEBUG
    cerr<<"proc_auth_info:1"<<endl;
#endif
    if(contype==smtp) {
            if( vqai.flags & cvq::smtp_blk ) {
                    olog->aw("blocked SMTP access", vqai.dom, vqai.user );
                    return(1);
            }
            if( (! resp.empty() && ! auth_cram())
                || pass==vqai.pass ) {
#ifdef DEBUG
                    cerr<<"proc_auth_info:2"<<endl;
#endif
                    return login_smtp();
            }
    } else {
            if( vqai.flags & cvq::pop3_blk ) {
                    olog->aw("blocked POP3 access", vqai.dom, vqai.user );
                    return(1);
            }
            if( (! resp.empty() && ! auth_apop())
                || pass==vqai.pass ) {
#ifdef DEBUG
                    cerr<<"proc_auth_info:3"<<endl;
#endif
                    return login_pop();
            }
    }
    olog->ae( "invalid login (pass="+pass+";resp="+resp+")", 
            vqai.dom, vqai.user );
    return(1);
}

/*
 *
 */
int main(int ac, char **av)
{
    me = *av;
    ::av = av;
    ::ac = ac;
    
    sig_pipeign();

    try {
            if(read_env()) return 3;
            cauth *auth=NULL;
#ifdef DEBUG
            cerr<<"main:1"<<endl;
#endif
            vq_init(&vq, &auth);
            log_init(&olog);
            olog->clip_set(remip);
#ifdef DEBUG
            cerr<<"main:2"<<endl;
#endif

            if(read_auth_info()) return 2;
            char ret;
#ifdef DEBUG
            cerr<<"main:3"<<endl;
#endif
            if( (ret=vq->user_auth(vqai)) ) {
                    olog->ae( (string)vq->err_str(ret)
                            +";AUTH="+auth->err()
                            +" ("+pass+")", vqai.dom, vqai.user );
                    return(1);
            }
#ifdef DEBUG
            cerr<<"main:4"<<endl;
#endif
            return proc_auth_info();
    } catch( const exception & e ) {
//            cerr << "exception: " << e.what() << endl;
            return(1);
    }
    return(0);
}
