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

#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <new>

#include "cusersmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "infodb_init.h"

using namespace std;

const char cusersmod::_id[] = "users";
const char cusersmod::_conf[] = "<users/><desc><users/></desc>";
const char cusersmod::_name[] = "";

const char * cusersmod::conf() {
  return ! (sessv.ai.flags &cvq::admin) ? "" : _conf;
}

/**
 *
 */
cusersmod::cusersmod() {
  modinfo = sessv.ai.flags & cvq::admin ? mi_conf : 0;
}
/***************************************************************************
 * 
 * *************************************************************************/
void cusersmod::act() {
  vector<cinfodb::user_info> users;
  vector<cinfodb::user_info>::size_type cnt, i;
  vector<cinfodb::user_info>::iterator ptr;
  *o << "<mod><users>";
  
  if( ! idb->user_ls(sessv.ai.dom, users) && users.size() ) {
    string uid, act, ver, user_sel;
    if ( cgi_var(cgi, "uid", uid) && cgi_var(cgi, "act", act) ) {
      cmp.id = uid;
      if ( (ptr = find_if(users.begin(), users.end(), cmp)) != users.end() ) {
        if ( "r" == act ) {
          if ( cgi_var(cgi, "ver", ver) && "1" == ver ) {
            if ( vq->user_rm(ptr->user, sessv.ai.dom) ) {
              *o << "<cant><rm/></cant>";
            } else {
              users.erase(ptr);
              *o << "<ok><rm/></ok>";
            }
          } else {
            *o <<"<ver><rm id=\""<< uid << "\"/></ver>";
            goto act_end;
          }
        }
      }
    }
    for( i=0, cnt = users.size(); i<cnt; ++i )
      *o <<"<item user=\""<< users[i].user 
         <<"\" id=\"" << users[i].id 
         <<"\" date_crt=\"" << users[i].date_crt
         <<"\" date_exp=\"" << users[i].date_exp << "\"/>";
  } else *o << "<cant><get/></cant>";

act_end:  
  *o << "</users></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cusersmod::run() {
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
