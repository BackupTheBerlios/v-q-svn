/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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

#include "cpassmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "vqwww_conf.h"
#include "html.h"

using namespace std;
using namespace vqwww;
using namespace cgicc;

const char cpassmod::_id[] = "pass";
const char cpassmod::_conf[] = "<pass/><desc><pass/></desc>";

void cpassmod::act() {
    out<< "<mod><pass>";
    if( env.cgi_rm_get() == cenvironment::rm_post ) {
            bool ok= true;
            string cpass, npass, rpass;
            string::size_type len;
            out<<"<inv>";
            out<<"<cpass";
            if( ! cgi_var(&env.cgi_get(), "cpass", cpass) || cpass.empty() ) {
                    ok = false;
                    out<<" empty=\"1\" "; 
            }
            out<<"/>";
            out<<"<npass";
            if( ! cgi_var(&env.cgi_get(), "npass", npass) || npass.empty() ) {
                    ok = false;
                    out<<" empty=\"1\" ";
            } else if( (len=npass.length()) < ac_pass_minl.val_int() ) {
                    ok = false;
                    out<<" tooshort=\"1\" ";
            } else if( len > ac_pass_maxl.val_int() ) {
                    ok = false;
                    out<<" toolong=\"1\" ";
            } else if( htmlbad(npass, ac_pass_ok.val_str().c_str(), 
					ac_html_br.val_str().at(0) ) ) {
                    ok = false;
                    out<< " dirty=\"1\" ";
            }
            out<<"/>";
            cgi_var(&env.cgi_get(), "rpass", rpass);
            if( rpass != npass ) {
                    ok = false;
                    out<<"<dontmatch/>";
            }
            if( ok ) {
					cvq::auth_info ai;
					ai.dom = env.sessb_get().ai.dom;
					ai.user = env.sessb_get().ai.user;
					
                    if( ! env.vq_get().user_auth(ai) ) {
							if( ai.pass != cpass ) {
									out<<"<oldbad/>";
							} else {
									if( ! env.vq_get().user_pass(
												env.sessb_get().ai.user, 
												env.sessb_get().ai.dom, npass ) ) {
											out<<"<done/>";
									} else {
											out<<"<chgcant/>";
											err<<"cpassmod: can't set password: "
												<<env.vq_get().err_report()<<endl;
									}
							}
                    } else {
                            out<<"<getcant/>";
                            err<<"cpassmod: can't get password: "
								<<env.vq_get().err_report()<<endl;
                    }
            }
            out<<"</inv>";
    }
    out<< "</pass></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cpassmod::run() {
    const_form_iterator fi = env.cgi_get().getElement("conf");
    if( fi != env.cgi_end_get() ) {
            fi = env.cgi_get().getElement("id");
            if( fi != env.cgi_end_get() && fi->getValue() == _id ) {
                    act();
                    return done;
            }
    }
    return notme;
}
