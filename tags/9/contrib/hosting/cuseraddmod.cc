/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
