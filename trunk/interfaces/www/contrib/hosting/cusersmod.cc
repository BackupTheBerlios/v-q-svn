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

#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <new>

#include "cusersmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "cinfodb.h"

using namespace std;

const char cusersmod::_id[] = "users";
const char cusersmod::_conf[] = "<users/><desc><users/></desc>";

const char * cusersmod::conf() {
  return ! (sessb.ai.flags &cvq::admin) ? "" : _conf;
}

/**
 *
 */
cusersmod::cusersmod( cenvironment & env ) : cmod(env) {
  modinfo = sessb.ai.flags & cvq::admin ? mi_conf : 0;
}
/***************************************************************************
 * 
 * *************************************************************************/
void cusersmod::act() {
  vector<cinfodb::user_info> users;
  vector<cinfodb::user_info>::size_type cnt, i;
  vector<cinfodb::user_info>::iterator ptr;
  out<< "<mod><users>";
  
  if( ! idb->user_ls(sessb.ai.dom, users) && users.size() ) {
    string uid, act, ver, user_sel;
    if ( cgi_var(cgi, "uid", uid) && cgi_var(cgi, "act", act) ) {
      cmp.id = uid;
      if ( (ptr = find_if(users.begin(), users.end(), cmp)) != users.end() ) {
        if ( "r" == act ) {
          if ( cgi_var(cgi, "ver", ver) && "1" == ver ) {
            if ( vq.user_rm(ptr->user, sessb.ai.dom) ) {
              out<< "<cant><rm/></cant>";
            } else {
              users.erase(ptr);
              out<< "<ok><rm/></ok>";
            }
          } else {
            out<<"<ver><rm id=\""<< uid << "\"/></ver>";
            goto act_end;
          }
        }
      }
    }
    for( i=0, cnt = users.size(); i<cnt; ++i )
      out<<"<item user=\""<< users[i].user 
         <<"\" id=\"" << users[i].id 
         <<"\" date_crt=\"" << users[i].date_crt
         <<"\" date_exp=\"" << users[i].date_exp << "\"/>";
  } else out<< "<cant><get/></cant>";

act_end:  
  out<< "</users></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cusersmod::run() {
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
