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
iMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
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

#include "cwwwstatmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "infodb_init.h"
#include "html.h"

using namespace std;

const char cwwwstatmod::_id[] = "wwwstat";
const char cwwwstatmod::_conf[] = ""; 
const char cwwwstatmod::_name[] = "wwwstat";
const char cwwwstatmod::_menu[] = "<wwwstat/>";

/**
 *
 */
void cwwwstatmod::item_dump( const string & pre ) {
    *o<<"<item prefix=\""<< pre << "\"/>";
    if(! domains.empty()) domains += ',';
    if( ! pre.empty() ) {
            domains += pre;
            domains += '.';
    }
    domains += domain;
    empty = false;
}

/***************************************************************************
 * 
 * *************************************************************************/
void cwwwstatmod::act() {
  vector<cinfodb::wwwali_dom_info> wadis;
  vector<cinfodb::wwwali_dom_info>::size_type cnt, i;

  if( !idb->wwwali_ls(sessv.ai.dom, wadis) && wadis.size() ) {
    cnt = wadis.size();
    if( ! cnt ) {
      *o << "<noent/>"; return;
    }
    for( i=0; i < cnt; i++ ) {
      domain = wadis[i].domain;
      *o<<"<dom name=\""<< wadis[i].domain <<"\">";
      item_dump( "" );
      act_dom(wadis[i].id);
      *o<<"</dom>";
    }
  } else *o << "<getcant/>";
}
/**
 *
 */
void cwwwstatmod::act_user() {
  cinfodb::ftp_info fi;
  fi.user = sessv.ai.user;
  switch(idb->ftp_get(sessv.ai.dom, fi)) {
  case cinfodb::err_noent: *o<<"<noent/>"; return;
  case cinfodb::err_no: break;
  default: *o<<"<getcant/>"; return;
  }

  if( fi.dir.empty() ) {
    *o<<"<noent/>"; return;
  }

  vector<cinfodb::wwwali_dom_info> wadis;
  vector<cinfodb::wwwali_dom_info>::size_type cnt, i;

  if( !idb->wwwali_ls(sessv.ai.dom, wadis) && wadis.size() ) {
    cnt = wadis.size();
    if( ! cnt ) {
      *o<<"<noent/>"; return;
    }
    for( i=0; i < cnt; i++ ) {
      domain = wadis[i].domain;
      *o<<"<dom name=\""<< wadis[i].domain <<"\">";
      if( fi.dir == "/" ) item_dump("");
      act_dom_dir(wadis[i].id, fi.dir);
      *o<<"</dom>";
    }
  } else *o << "<getcant/>";
}

/**
 *
 */
void cwwwstatmod::act_dom_dir(const string &domid, const string &dir) {
  vector<cinfodb::wwwali_info> wais;
  vector<cinfodb::wwwali_info>::size_type cnt, i;
  if( ! idb->wwwali_dom_ls_dir( domid, dir, wais ) ) {
    cnt = wais.size();
    if( ! cnt ) {
      *o<<"<noent/>";
      return;
    }
    for( i=0; i<cnt; ++i )
      item_dump( wais[i].prefix );
  } else *o << "<getcant/>";
}
/**
 *
 */
void cwwwstatmod::act_dom( const string & di ) {
 vector<cinfodb::wwwali_info> wais;
 vector<cinfodb::wwwali_info>::size_type cnt, i;

 if( ! idb->wwwali_dom_ls( di, wais ) ) {
   cnt = wais.size();
   if( ! cnt ) {
     *o<<"<noent/>";
     return;
   }
   for( i=0; i<cnt; ++i )
     item_dump( wais[i].prefix );
 } else *o << "<getcant/>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cwwwstatmod::run() {
  try {
    infodb_init(idb);
  } catch(...) {
    return notme;
  }
  const_form_iterator fi = cgi->getElement("id");
  if( fi != cgie && fi->getValue() == _id ) {
    domains = "";
    empty = true;
    *o << "<mod><wwwstat sid=\"" << sess->getid() << "\">";
    
    if ( sessv.ai.flags & cvq::admin ) act();
    else act_user();
    
    if( empty ) *o << "<noent/>";
    *o << "</wwwstat></mod>";
    sess->setval("stat_domains", domains);
    return done;
  }
  return notme;
}
