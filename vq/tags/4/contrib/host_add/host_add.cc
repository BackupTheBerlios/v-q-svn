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

#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cgicc/Cgicc.h>
#include <deque>
#include <fstream>
#include <cerrno>
#include <algorithm>

#include <cauth.h>
#include <cvq.h>

#include "cgi.h"
#include "host_add_conf.h"
#include "html.h"
#include "sabmain.h"

using namespace std;
using namespace cgicc;
using namespace host_add;

Cgicc *cgi=NULL;
const_form_iterator cgie;
const CgiEnvironment *ce = NULL;

ostream *o=NULL, *e=NULL;

/**
 *
 */
void get_domains( deque<string> & domains ) {
    ifstream in(ac_domains.val_str().c_str());
    if(!in) throw runtime_error((string)"can't open: "+ac_domains.val_str()
		+": "+strerror(errno));
    string ln;
    while(getline(in, ln)) {
            domains.push_back(ln);
    }
    if(in.bad()) 
            throw runtime_error((string)"error while reading: "+ac_domains.val_str()
				+": "+strerror(errno));
}

/**
 *
 */
void start( int ac, char **av, ostream & o, ostream & e, xslt_args_type & )
{
    ::o = &o;
    ::e = &e;
    cgi = new Cgicc();
    cgie = cgi->getElements().end();
    ce = & cgi->getEnvironment();
    string user, pass, pass1, domain, pref;
    deque<string> domains;
    cgi_var(cgi, "pref", pref);
    cgi_var(cgi, "user", user);
    cgi_var(cgi, "domain", domain );
    cgi_var(cgi, "pass", pass);
    cgi_var(cgi, "pass1", pass1);
    get_domains(domains);

    o<<"<host_add>";
    if( stringsAreEqual(ce->getRequestMethod(),"POST") ) {
            bool ok = true;
            string::size_type len;
            o<<"<inv><user ";
            len = user.length();
            if(len<3) {
                    o<<"tooshort=\"1\" ";
                    ok = false;
            } else if(len>30) {
                    o<<"toolong=\"1\" ";
                    ok = false;
            } 
            if(htmlbad(user, ac_user_ok.val_str().c_str(), ac_html_br.val_str().at(0))) {
                    o<<"dirty=\"1\" ";
                    ok = false;
            }
            o<<"/><pref ";
            len = pref.length();
            if(len<3) {
                    o<<"tooshort=\"1\" ";
                    ok = false;
            } else if(len>30) {
                    o<<"toolong=\"1\" ";
                    ok = false;
            } 
            if(htmlbad(pref, ac_pref_ok.val_str().c_str(), ac_html_br.val_str().at(0))) {
                    o<<"dirty=\"1\" ";
                    ok = false;
            }
            o<<"/>";

            if(find(domains.begin(), domains.end(), domain)==domains.end()) {
                    ok = false;
                    domain = domains[0];
                    o<<"<domain/>";
            }
            o<<"<pass ";
            len = pass.length();
            if(len<3) {
                    o<<"tooshort=\"1\" ";
                    ok = false;
            } else if(len>15) {
                    o<<"toolong=\"1\" ";
                    ok = false;
            } 
            if(htmlbad(pass, ac_pass_ok.val_str().c_str(), ac_html_br.val_str().at(0))) {
                    o<<"dirty=\"1\" ";
                    ok = false;
            }
            if( pass != pass1 ) {
                    o<<" nomatch=\"1\" ";
            }
            o<<"/></inv>";
            if( ok ) {
                    auto_ptr<cvq> vq(cvq::alloc());
                    int ret;
					
					domain = pref+(pref[pref.length()-1] == '.' ? "" : ".")
						+domain;
                    if( (ret=vq->dom_add(domain)) ) {
                            o<<"<add err=\"\"/>";
                            e<<*av<<": vq->dom_add: "<<vq->err_str(ret)<<": "<<vq->err_report()<<endl;
                    } else {
							if( (ret=vq->user_add(user, domain, pass)) ) {
                            		o<<"<add err=\"\"/>";
		                            e<<*av<<": vq->user_add: "<<vq->err_str(ret)<<": "<<vq->err_report()<<endl;
		                    } else {
		                            o<<"<add ok=\"\"/></host_add>";
		                            return;
		                    }
					}
            }
    }
    o<<"<pref val=\"" << pref << "\"/>";
    o<<"<domains>";
    for(unsigned i=0, s = domains.size(); i<s; ++i ) {
            o<<"<domain val=\"" << domains[i] << "\"";
            if( domain==domains[i] ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</domains>";
    o<<"<user val=\"" << user << "\"/>";
    o<<"<pass val=\"" << pass << "\"/>";
    o<<"<pass1 val=\"" << pass1 << "\"/>";
    o<<"</host_add>";
}
