/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

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

#include <vqwww.h>
#include <cgi.h>
#include <html.h>
#include <vqwww_conf.h>

#include "cftpmod.h"
#include "cinfodb.h"
#include "hosting_conf.h"

using namespace std;
using namespace hosting;
using namespace vqwww;

const char cftpmod::_id[] = "ftp";
const char cftpmod::_conf[] = "<ftp/><desc><ftp/></desc>";
const char cftpmod::_name[] = "";

const char * cftpmod::conf() {
  return ! (sessb.ai.flags &cvq::admin) ? "" : _conf;
}

/**
 *
 */
cftpmod::cftpmod( cenvironment & env ) : cmod(env) {
  modinfo = sessb.ai.flags & cvq::admin ? mi_conf : 0;
}
/****************************************************************************
 * 
 * **************************************************************************/
void cftpmod::rmspace(string &s) {
 string::iterator ptr=s.begin(), end=s.end();
 while( ptr != end ) {
   if( isspace(*ptr) ) {
     s.erase(ptr);
     end = s.end();
   } else ptr ++;
 }
}
/***************************************************************************
 * 
 * *************************************************************************/
void cftpmod::act() {
  vector<cinfodb::ftp_info> users;
  vector<cinfodb::ftp_info>::size_type cnt, i;
  out<< "<mod><ftp>";
 
 if( ! idb->ftp_ls( sessb.ai.dom, users) && users.size() ) {
   if( env.cgi_rm_get() == cenvironment::rm_post ) {
     bool ok= true;
     string val, id, user;
     vector<cinfodb::ftp_info> torep, torm(users);
     vector<cinfodb::ftp_info>::iterator ptr;
     cinfodb::ftp_info uitmp;
     ostringstream os;
     
     out<<"<inv>";
     // we get list of items
     for( cnt=users.size(), i=1; i <= cnt; i++ ) {
       os.str("");
       os<<i;
       if( cgi_var(cgi, "itemid"+os.str(), id) 
          && cgi_var(cgi, "itemuser"+os.str(), user) ) {
         cgi_var(cgi, "item"+os.str(), val);
         
         rmspace(user);
         if(user.empty()) continue;
         rmspace(val);
         rmspace(id);

         cmp.id = id;
         ptr = find_if(torm.begin(), torm.end(), cmp);
         if( ptr == torm.end() ) continue; // not found

         if( ! val.empty() ) {
           if ( ptr->dir != val ) {
             uitmp.dir = val;
             uitmp.id = id;
             uitmp.user = user;
             torep.push_back(uitmp);
           }
           torm.erase(ptr);
         } else if( val == ptr->dir ) {
           torm.erase(ptr);
         }
       }
     }
     if( ok ) {
       for( i=0, cnt=torep.size(); i<cnt; ++i ) {
         if( htmlbad(torep[i].dir, ac_ftp_ok.val_str().c_str(), 
				ac_html_br.val_str().at(0)) ) {
           out<<"<dirchars user=\""<< torep[i].user 
             <<"\" dir=\""<< torep[i].dir <<"\"/>";
         } else if( ! valid(torep[i].dir) ) {
           out<<"<dir user=\""<<torep[i].user
             <<"\" dir=\""<< torep[i].dir <<"\"/>";
         } else if(idb->ftp_rep(sessb.ai.dom, torep[i])) {
           err<<"cftpmod::ftp_rep: "<<vq.err_report()<<endl;
           out<<"<rep user=\""<<torep[i].user
             <<"\" dir=\""<< torep[i].dir <<"\"/>";
         }
         cmp.id = torep[i].id;
         ptr = find_if(users.begin(), users.end(), cmp);
         if( ptr != users.end() ) *ptr = torep[i];
       }
       for( i=0, cnt=torm.size(); i<cnt; ++i ) {
         if(idb->ftp_rm(sessb.ai.dom, torm[i].id)) {
           err<<"cftpmod::ftp_rm"<<vq.err_report()<<endl;
           out<<"<rm user=\""<<torm[i].user
             <<"\" dir=\""<< torm[i].dir <<"\"/>";
           continue;
         }
         cmp.id = torm[i].id;
         ptr = find_if(users.begin(), users.end(), cmp);
         torm[i].dir = "";
         if( ptr != users.end() ) *ptr = torm[i];
       }
     }
     out<<"</inv>";
   }
   for( i=0, cnt = users.size(); i<cnt; ++i )
     out<<"<item user=\""<< users[i].user 
        <<"\" val=\"" << users[i].dir << "\" id=\""
        << users[i].id << "\"/>";
 } else out<< "<getcant/>";
 out<< "</ftp></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cftpmod::run() {
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
 *
 *
 */
bool cftpmod::valid(const string &dir) {
  if( dir[0]=='.' && dir[1] == '.' && dir[2] == '/' ) return false;
  string::size_type len = dir.length();
  if( dir[len-3] == '/' && dir[len-2] == '.' && dir[len-1] == '.' ) return false;
  if( dir.find("/../") != string::npos ) return false;
  return true;
}
