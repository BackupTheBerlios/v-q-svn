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

#include <vqwww.h>
#include <cgi.h>
#include <html.h>
#include <vqwww_conf.h>

#include "cwwwalimod.h"
#include "cinfodb.h"
#include "hosting_conf.h"

using namespace std;
using namespace hosting;
using namespace vqwww;

const char cwwwalimod::_id[] = "wwwali";
const char cwwwalimod::_conf[] = "<wwwali/><desc><wwwali/></desc>";

const char *cwwwalimod::conf() {
  return ! (sessb.ai.flags & cvq::admin) ? "" : _conf;
}

/**
 *
 */
cwwwalimod::cwwwalimod( cenvironment & env ) : cmod(env) {
    modinfo = sessb.ai.flags & cvq::admin ? mi_conf : 0;
}
/***************************************************************************
 *
 * *************************************************************************/
void cwwwalimod::rmspace(string &s) {
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
void cwwwalimod::act() {
  vector<cinfodb::wwwali_dom_info> wadis;
  vector<cinfodb::wwwali_dom_info>::size_type cnt, i;

  out<< "<mod><wwwali>";
  if( !idb->wwwali_ls(sessb.ai.dom, wadis) && wadis.size() ) {
    bool domidok = true;
    cgi_var(cgi, "domid", domid);
    if(domid.empty()) { domidok = false; domid = wadis[0].id; }
    out<<"<domid id=\""<< domid <<"\"/>";
    for( i=0, cnt = wadis.size(); i < cnt; i++ ) {
      out<<"<dom id=\""<< wadis[i].id <<"\" name=\""<< wadis[i].domain <<"\"";
      if(wadis[i].id == domid) out<< " sel=\"1\" ";
      out<< "/>";
    }
    act_dom(domidok);
  } else out<< "<getcant/>";
  out<< "</wwwali></mod>";
}
/**
 *
 */
void cwwwalimod::act_dom( bool domidok ) {
 vector<cinfodb::wwwali_info> wais;
 vector<cinfodb::wwwali_info>::size_type cnt, i;
 uint8_t cnt_add = 5;// always add n empty boxes

 if( ! idb->wwwali_dom_ls( domid, wais ) ) {
   if( env.cgi_rm_get() == cenvironment::rm_post && domidok ) {
     bool ok= true;
     string prefix, val, id;
     vector<cinfodb::wwwali_info> torep, torm(wais), toadd;
     vector<cinfodb::wwwali_info>::iterator ptr;
     cinfodb::wwwali_info uitmp;
     ostringstream os;
     
     out<<"<inv>";
     // we get list of items
     for( cnt=wais.size()+cnt_add, i=1; i <= cnt; i++ ) {
       os.str("");
       os<<i;
       if( cgi_var(cgi, "itempre"+os.str(), prefix) ) {
         cgi_var(cgi, "itemid"+os.str(), id);
         cgi_var(cgi, "item"+os.str(), val);
         
         rmspace(prefix);
         if(prefix.empty()) continue;
         rmspace(val);
         if(val.empty()) val = '/';
         rmspace(id);

         cmp.id = id;
         if( ! cmp.id.empty()
            && (ptr=find_if(torm.begin(), torm.end(), cmp)) != torm.end() ) {
           if ( ptr->dir != val || ptr->prefix != prefix ) {
             uitmp.dir = val;
             uitmp.id = id;
             uitmp.prefix = prefix;
             torep.push_back(uitmp);
           }
           torm.erase(ptr);
         } else {
           uitmp.dir = val;
           uitmp.prefix = prefix;
           toadd.push_back(uitmp);
         }
       }
     }
     if( ok ) {
       for( i=0, cnt=toadd.size(); i<cnt; ++i ) {
         if( chk(toadd[i]) && idb->wwwali_add(domid, toadd[i])) {
           err<<"cwwwalimod::wwwali_add: "<<vq.err_report()<<endl;
           out<<"<add prefix=\""<< toadd[i].prefix 
             <<"\" dir=\""<< toadd[i].dir <<"\"/>";
         }
         wais.push_back(toadd[i]);
       }
       for( i=0, cnt=torep.size(); i<cnt; ++i ) {
         if( chk(torep[i]) && idb->wwwali_rep(domid, torep[i])) {
           err<<"cwwwalimod::wwwali_rep: "<<vq.err_report()<<endl;
           out<<"<rep prefix=\""<<torep[i].prefix
             <<"\" dir=\""<< torep[i].dir <<"\"/>";
         }
         cmp.id = torep[i].id;
         ptr = find_if(wais.begin(), wais.end(), cmp);
         if( ptr != wais.end() ) *ptr = torep[i];
       }
       for( i=0, cnt=torm.size(); i<cnt; ++i ) {
         if(idb->wwwali_rm(domid, torm[i].id)) {
           err<<"cwwwalimod::wwwali_rm: "<<vq.err_report()<<endl;
           out<<"<rm user=\""<<torm[i].prefix
             <<"\" dir=\""<< torm[i].dir <<"\"/>";
           continue;
         }
         cmp.id = torm[i].id;
         ptr = find_if(wais.begin(), wais.end(), cmp);
         if( ptr != wais.end() ) wais.erase(ptr);
       }
     }
     out<<"</inv>";
   }
   for( i=0, cnt = wais.size(); i<cnt; ++i )
     out<<"<item prefix=\""<< wais[i].prefix 
        <<"\" val=\"" << wais[i].dir << "\" id=\""
        << wais[i].id << "\"/>";
   for( cnt+=cnt_add; i < cnt; i++ )
     out<< "<item/>";
 } else out<< "<getcant/>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cwwwalimod::run() {
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
bool cwwwalimod::valid(const string &dir) {
  if( dir[0]=='.' && dir[1] == '.' && dir[2] == '/' ) return false;
  string::size_type len = dir.length();
  if( dir[len-3] == '/' && dir[len-2] == '.' && dir[len-1] == '.' ) return false;
  if( dir.find("/../") != string::npos ) return false;
  return true;
}

/**
 *
 */
bool cwwwalimod::chk( cinfodb::wwwali_info & wai ) {
  if( htmlbad(wai.prefix, ac_prefix_ok.val_str().c_str(), 
	ac_html_br.val_str().at(0))) {
    out<<"<prefixchars prefix=\""<< wai.prefix
      <<"\" dir=\""<< wai.dir <<"\"/>";
    return false;
  } 
  if( htmlbad(wai.dir, ac_ftp_ok.val_str().c_str(), ac_html_br.val_str().at(0)) ) {
    out<<"<dirchars prefix=\""<< wai.prefix 
      <<"\" dir=\""<< wai.dir <<"\"/>";
    return false;
  } 
  if( ! valid(wai.dir) ) {
    out<<"<dir prefix=\""<<wai.prefix
      <<"\" dir=\""<< wai.dir <<"\"/>";
    return false;
  }
  return true;
}

