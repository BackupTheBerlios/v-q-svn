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
