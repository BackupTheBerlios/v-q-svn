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

#include "cwwwalimod.h"
#include "vqwww.h"
#include "cgi.h"
#include "infodb_init.h"
#include "conf_html_br.h"
#include "conf_ftp_ok.h"
#include "conf_prefix_ok.h"
#include "html.h"

using namespace std;

const char cwwwalimod::_id[] = "wwwali";
const char cwwwalimod::_conf[] = "<wwwali/><desc><wwwali/></desc>";
const char cwwwalimod::_name[] = "";

const char *cwwwalimod::conf() {
  return ! (sessv.ai.flags & cvq::admin) ? "" : _conf;
}

/**
 *
 */
cwwwalimod::cwwwalimod() {
    modinfo = sessv.ai.flags & cvq::admin ? mi_conf : 0;
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

  *o << "<mod><wwwali>";
  if( !idb->wwwali_ls(sessv.ai.dom, wadis) && wadis.size() ) {
    bool domidok = true;
    cgi_var(cgi, "domid", domid);
    if(domid.empty()) { domidok = false; domid = wadis[0].id; }
    *o<<"<domid id=\""<< domid <<"\"/>";
    for( i=0, cnt = wadis.size(); i < cnt; i++ ) {
      *o<<"<dom id=\""<< wadis[i].id <<"\" name=\""<< wadis[i].domain <<"\"";
      if(wadis[i].id == domid) *o << " sel=\"1\" ";
      *o<< "/>";
    }
    act_dom(domidok);
  } else *o << "<getcant/>";
  *o << "</wwwali></mod>";
}
/**
 *
 */
void cwwwalimod::act_dom( bool domidok ) {
 vector<cinfodb::wwwali_info> wais;
 vector<cinfodb::wwwali_info>::size_type cnt, i;
 uint8_t cnt_add = 5;// always add n empty boxes

 if( ! idb->wwwali_dom_ls( domid, wais ) ) {
   if( reqmet == post && domidok ) {
     bool ok= true;
     string prefix, val, id;
     vector<cinfodb::wwwali_info> torep, torm(wais), toadd;
     vector<cinfodb::wwwali_info>::iterator ptr;
     cinfodb::wwwali_info uitmp;
     ostringstream os;
     
     *o<<"<inv>";
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
           *e<<"cwwwalimod::wwwali_add: "<<vq->err()<<endl;
           *o<<"<add prefix=\""<< toadd[i].prefix 
             <<"\" dir=\""<< toadd[i].dir <<"\"/>";
         }
         wais.push_back(toadd[i]);
       }
       for( i=0, cnt=torep.size(); i<cnt; ++i ) {
         if( chk(torep[i]) && idb->wwwali_rep(domid, torep[i])) {
           *e<<"cwwwalimod::wwwali_rep: "<<vq->err()<<endl;
           *o<<"<rep prefix=\""<<torep[i].prefix
             <<"\" dir=\""<< torep[i].dir <<"\"/>";
         }
         cmp.id = torep[i].id;
         ptr = find_if(wais.begin(), wais.end(), cmp);
         if( ptr != wais.end() ) *ptr = torep[i];
       }
       for( i=0, cnt=torm.size(); i<cnt; ++i ) {
         if(idb->wwwali_rm(domid, torm[i].id)) {
           *e<<"cwwwalimod::wwwali_rm: "<<vq->err()<<endl;
           *o<<"<rm user=\""<<torm[i].prefix
             <<"\" dir=\""<< torm[i].dir <<"\"/>";
           continue;
         }
         cmp.id = torm[i].id;
         ptr = find_if(wais.begin(), wais.end(), cmp);
         if( ptr != wais.end() ) wais.erase(ptr);
       }
     }
     *o<<"</inv>";
   }
   for( i=0, cnt = wais.size(); i<cnt; ++i )
     *o <<"<item prefix=\""<< wais[i].prefix 
        <<"\" val=\"" << wais[i].dir << "\" id=\""
        << wais[i].id << "\"/>";
   for( cnt+=cnt_add; i < cnt; i++ )
     *o << "<item/>";
 } else *o << "<getcant/>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cwwwalimod::run() {
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
  if( htmlbad(wai.prefix, conf_prefix_ok.c_str(), conf_html_br[0])) {
    *o<<"<prefixchars prefix=\""<< wai.prefix
      <<"\" dir=\""<< wai.dir <<"\"/>";
    return false;
  } 
  if( htmlbad(wai.dir, conf_ftp_ok.c_str(), conf_html_br[0]) ) {
    *o<<"<dirchars prefix=\""<< wai.prefix 
      <<"\" dir=\""<< wai.dir <<"\"/>";
    return false;
  } 
  if( ! valid(wai.dir) ) {
    *o<<"<dir prefix=\""<<wai.prefix
      <<"\" dir=\""<< wai.dir <<"\"/>";
    return false;
  }
  return true;
}

