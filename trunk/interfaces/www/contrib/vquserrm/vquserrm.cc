/**
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
