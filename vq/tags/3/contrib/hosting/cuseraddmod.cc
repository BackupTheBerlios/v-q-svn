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
#include "infodb_init.h"

using namespace std;
using namespace hosting;

const char cuseraddmod::_id[] = "useradd";
const char cuseraddmod::_conf[] = "<useradd/><desc><useradd/></desc>";
const char cuseraddmod::_name[] = "";

const char * cuseraddmod::conf() {
          return ! (sessv.ai.flags &cvq::admin) ? "" : _conf;
}
/**
 *
 */
cuseraddmod::cuseraddmod() {
  modinfo = sessv.ai.flags & cvq::admin ? mi_conf : 0;
}
/**
 *
 */
void cuseraddmod::act() {
    *o << "<mod><useradd>";

    string login;
    if( reqmet == post ) {
            bool ok=true;
            string pass, pass1;
            string::size_type len;
            *o<<"<inv>";
            *o<<"<login";
            if( ! cgi_var(cgi, "login", login) || login.empty() ) {
                    ok = false;
                    *o<<" empty=\"1\" "; 
            } else if( (len=login.length()) < ac_user_minl.val_int() ) {
                    ok = false;
                    *o <<" tooshort=\"1\" ";
            } else if( len > ac_user_maxl.val_int() ) {
                    ok = false;
                    *o<<" toolong=\"1\" ";
            } else if( htmlbad(pass, ac_user_ok.val_str().c_str(), 
				ac_html_br.val_str().at(0) ) ) {
                    ok = false;
                    *o << " dirty=\"1\" ";
            }
            *o<<"/>";
            *o<<"<pass";
            if( ! cgi_var(cgi, "pass", pass) || pass.empty() ) {
                    ok = false;
                    *o<<" empty=\"1\" ";
            } else if( (len=pass.length()) < ac_pass_minl.val_int() ) {
                    ok = false;
                    *o<<" tooshort=\"1\" ";
            } else if( len > ac_pass_maxl.val_int() ) {
                    ok = false;
                    *o<<" toolong=\"1\" ";
            } else if( htmlbad(pass, ac_pass_ok.val_str().c_str(), 
				ac_html_br.val_str().at(0) ) ) {
                    ok = false;
                    *o << " dirty=\"1\" ";
            }
            *o<<"/>";
            cgi_var(cgi, "pass1", pass1);
            if( pass != pass1 ) {
                    ok = false;
                    *o<<"<dontmatch/>";
            }
            if( ok ) {
                    uint8_t ret = vq->user_add(login, sessv.ai.dom, pass );
                    switch(ret) {
                    case cvq::err_user_ex:
                            *o<<"<exists/>";
                            break;
                    case cvq::err_user_inv:
                            *o<<"<inv/>";
                            break;
                    case cvq::err_no: 
                            *o<<"<done/>";
                            if( ! ac_notify.val_str().empty() )
                                    notify_run(login);
                            login = pass = pass1 = "";
                            break;
                    default:                            
                            *o<<"<cant/>";
                            *e<<"cuseraddmod: can't add user: "<<vq->err()<<endl;
                    }
            }
            *o<<"</inv>";
    }
    *o << "<login val=\"" << login << "\"/></useradd></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cuseraddmod::run() {
    if( ! (sessv.ai.flags & cvq::admin) ) return notme;
    try {
            infodb_init(idb);
    } catch(...) {
            return notme;
    }
    const_form_iterator fi = cgi->getElement("conf");
    if( fi != cgie ) {
            fi = cgi->getElement("id");
            if( fi != cgie && fi->getValue() == _id ) {
                    act();
                    return done;
            }
    }
    return notme;
}

/**
 */
void cuseraddmod::notify_run(const string &login) {
    string rcpt = sessv.ai.user+'@'+sessv.ai.dom;
    string ui_id;
    if( idb->ui_id_get(sessv.ai.dom, sessv.ai.user, ui_id) ) return;
   
	string notify(ac_notify.val_str());
    char *av[] = { 
            const_cast<char *>(notify.c_str()), 
            (char *)sessv.ai.dom.c_str(), 
            (char *)login.c_str(), 
            (char *)ui_id.c_str(), 
            (char *)rcpt.c_str(), NULL };

    int pid;
    switch((pid=vfork())) {
    case -1:
      *e<<"cuseraddmod::notify_run: vfork: "<<strerror(errno)<<endl;
      break;
    case 0:
      execvp( *av, av );
      *e<<"cuseraddmod::notify_run: execvp: "<< *av <<": "<<strerror(errno)<<endl;
      _exit(1);
    }
    while( wait(&pid) == -1 && EINTR == errno );
}
