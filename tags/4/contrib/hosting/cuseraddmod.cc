/*
Copyright (c) 2003 Pawel Niewiadomski
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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include <vqwww.h>
#include <cgi.h>
#include <html.h>
#include <vqwww_conf.h>

#include "cuseraddmod.h"
#include "hosting_conf.h"
#include "cinfodb.h"

using namespace std;
using namespace hosting;
using namespace vqwww;

const char cuseraddmod::_id[] = "useradd";
const char cuseraddmod::_conf[] = "<useradd/><desc><useradd/></desc>";

const char * cuseraddmod::conf() {
          return ! (sessb.ai.flags &cvq::admin) ? "" : _conf;
}
/**
 *
 */
cuseraddmod::cuseraddmod( cenvironment & env ) : cmod(env) {
  modinfo = sessb.ai.flags & cvq::admin ? mi_conf : 0;
}
/**
 *
 */
void cuseraddmod::act() {
    out<< "<mod><useradd>";

    string login;
    if( env.cgi_rm_get() == cenvironment::rm_post ) {
            bool ok=true;
            string pass, pass1;
            string::size_type len;
            out<<"<inv>";
            out<<"<login";
            if( ! cgi_var(cgi, "login", login) || login.empty() ) {
                    ok = false;
                    out<<" empty=\"1\" "; 
            } else if( (len=login.length()) < ac_user_minl.val_int() ) {
                    ok = false;
                    out<<" tooshort=\"1\" ";
            } else if( len > ac_user_maxl.val_int() ) {
                    ok = false;
                    out<<" toolong=\"1\" ";
            } else if( htmlbad(pass, ac_user_ok.val_str().c_str(), 
				ac_html_br.val_str().at(0) ) ) {
                    ok = false;
                    out<< " dirty=\"1\" ";
            }
            out<<"/>";
            out<<"<pass";
            if( ! cgi_var(cgi, "pass", pass) || pass.empty() ) {
                    ok = false;
                    out<<" empty=\"1\" ";
            } else if( (len=pass.length()) < ac_pass_minl.val_int() ) {
                    ok = false;
                    out<<" tooshort=\"1\" ";
            } else if( len > ac_pass_maxl.val_int() ) {
                    ok = false;
                    out<<" toolong=\"1\" ";
            } else if( htmlbad(pass, ac_pass_ok.val_str().c_str(), 
				ac_html_br.val_str().at(0) ) ) {
                    ok = false;
                    out<< " dirty=\"1\" ";
            }
            out<<"/>";
            cgi_var(cgi, "pass1", pass1);
            if( pass != pass1 ) {
                    ok = false;
                    out<<"<dontmatch/>";
            }
            if( ok ) {
                    uint8_t ret = vq.user_add(login, sessb.ai.dom, pass );
                    switch(ret) {
                    case cvq::err_user_inv:
                            out<<"<inv/>";
                            break;
                    case cvq::err_no: 
                            out<<"<done/>";
                            if( ! ac_notify.val_str().empty() )
                                    notify_run(login);
                            login = pass = pass1 = "";
                            break;
                    default:                            
                            out<<"<cant/>";
                            err<<"cuseraddmod: can't add user: "<<vq.err_report()<<endl;
                    }
            }
            out<<"</inv>";
    }
    out<< "<login val=\"" << login << "\"/></useradd></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cuseraddmod::run() {
    if( ! (sessb.ai.flags & cvq::admin) ) return notme;
	
	cgicc::const_form_iterator fi = cgi.getElement("conf");
    if( fi != env.cgi_end_get() ) {
            fi = cgi.getElement("id");
            if( fi != env.cgi_end_get() && fi->getValue() == _id ) {
            		idb.reset(cinfodb::alloc());

                    act();
                    return done;
            }
    }
    return notme;
}

/**
 */
void cuseraddmod::notify_run(const string &login) {
    string rcpt = sessb.ai.user+'@'+sessb.ai.dom;
    string ui_id;
    if( idb->ui_id_get(sessb.ai.dom, sessb.ai.user, ui_id) ) return;
   
	string notify(ac_notify.val_str());
    char *av[] = { 
            const_cast<char *>(notify.c_str()), 
            (char *)sessb.ai.dom.c_str(), 
            (char *)login.c_str(), 
            (char *)ui_id.c_str(), 
            (char *)rcpt.c_str(), NULL };

    int pid;
    switch((pid=vfork())) {
    case -1:
      err<<"cuseraddmod::notify_run: vfork: "<<strerror(errno)<<endl;
      break;
    case 0:
      execvp( *av, av );
      err<<"cuseraddmod::notify_run: execvp: "<< *av <<": "<<strerror(errno)<<endl;
      _exit(1);
    }
    while( wait(&pid) == -1 && EINTR == errno );
}
