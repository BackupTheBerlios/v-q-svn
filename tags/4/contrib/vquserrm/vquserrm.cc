/**
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

#include <memory>
#include <unistd.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cgicc/Cgicc.h>
#include <deque>
#include <fstream>
#include <cerrno>
#include <algorithm>
#include <sstream>

#include <cvq.h>
#include <util.h>
#include <sig.h>
#include <valid.h>
#include <lower.h>

#include "cgi.h"
#include "html.h"
#include "sabmain.h"

using namespace std;
using namespace cgicc;

const char *me= NULL;

Cgicc *cgi=NULL;
const_form_iterator cgie;
const CgiEnvironment *ce = NULL;

ostream *o=NULL, *e=NULL;

string email, pass;

/**
*/
bool post_act() {
    auto_ptr<cvq> vq;
    string::size_type atpos;
    cvq::auth_info ai;

    atpos = email.find('@');
    if( atpos == string::npos ) {
      *o<<"<inv/>";
      return false;
    }
    ai.user = email.substr(0, atpos);
    ai.dom = email.substr(atpos+1);
    if(ai.user.empty() || ai.dom.empty()) {
      *o<<"<inv/>";
      return false;
    }

    vq.reset(cvq::alloc());

    if( vq->user_auth(ai) == cvq::err_no ) {
      if( ai.pass == pass ) {
        if( vq->user_rm(ai.user, ai.dom) == cvq::err_no ) {
          *o<<"<removed/>";
          return true;
        } else {
          *o<<me<<": user_rm: "<<vq->err_report()<<endl;
        }
      }
    } else {
      *e<<me<<": user_auth: "<<vq->err_report()<<endl;
    }
    *o<<"<inv/>";
    return false;
}

/**
 *
 */
void start( int ac, char **av, ostream & o, ostream & e, xslt_args_type & ) 
{
    // setup
    me = *av;
    ::o = &o;
    ::e = &e;
   
    cgi = new Cgicc();
    cgie = cgi->getElements().end();
    ce = & cgi->getEnvironment();
    
    o<<"<vquserrm>";
 
    // real action
    cgi_var(cgi, "email", email); email=lower(email);
    cgi_var(cgi, "pass", pass);
#define spec_dec(x) x = html_spec_dec(x)
    spec_dec(email);
#undef spec_dec

    if( stringsAreEqual(ce->getRequestMethod(),"POST") ) {
            if( post_act() ) goto start_end;
    }
   
#define spec_enc(x) o<<"<" #x " val=\"" << html_spec_enc(x) << "\"/>"
    spec_enc(email);
#undef spec_enc
    o<<"<email val=\""<< email <<"\"/>";

start_end:
    o<<"</vquserrm>";
}
