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

#include "cwwwstatmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "cinfodb.h"
#include "html.h"

using namespace std;

const char cwwwstatmod::_id[] = "wwwstat";
const char cwwwstatmod::_conf[] = ""; 
const char cwwwstatmod::_name[] = "wwwstat";
const char cwwwstatmod::_menu[] = "<wwwstat/>";

/**
 *  
 */
cwwwstatmod::cwwwstatmod(cenvironment & env) : cmod(env) {
	modinfo = mi_menu;
}

/**
 *
 */
void cwwwstatmod::item_dump( const string & pre ) {
    out<<"<item prefix=\""<< pre << "\"/>";
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

  if( !idb->wwwali_ls(sessb.ai.dom, wadis) && wadis.size() ) {
    cnt = wadis.size();
    if( ! cnt ) {
      out<< "<noent/>"; return;
    }
    for( i=0; i < cnt; i++ ) {
      domain = wadis[i].domain;
      out<<"<dom name=\""<< wadis[i].domain <<"\">";
      item_dump( "" );
      act_dom(wadis[i].id);
      out<<"</dom>";
    }
  } else out<< "<getcant/>";
}
/**
 *
 */
void cwwwstatmod::act_user() {
  cinfodb::ftp_info fi;
  fi.user = sessb.ai.user;
  switch(idb->ftp_get(sessb.ai.dom, fi)) {
  case cinfodb::err_noent: out<<"<noent/>"; return;
  case cinfodb::err_no: break;
  default: out<<"<getcant/>"; return;
  }

  if( fi.dir.empty() ) {
    out<<"<noent/>"; return;
  }

  vector<cinfodb::wwwali_dom_info> wadis;
  vector<cinfodb::wwwali_dom_info>::size_type cnt, i;

  if( !idb->wwwali_ls(sessb.ai.dom, wadis) && wadis.size() ) {
    cnt = wadis.size();
    if( ! cnt ) {
      out<<"<noent/>"; return;
    }
    for( i=0; i < cnt; i++ ) {
      domain = wadis[i].domain;
      out<<"<dom name=\""<< wadis[i].domain <<"\">";
      if( fi.dir == "/" ) item_dump("");
      act_dom_dir(wadis[i].id, fi.dir);
      out<<"</dom>";
    }
  } else out<< "<getcant/>";
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
      out<<"<noent/>";
      return;
    }
    for( i=0; i<cnt; ++i )
      item_dump( wais[i].prefix );
  } else out<< "<getcant/>";
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
     out<<"<noent/>";
     return;
   }
   for( i=0; i<cnt; ++i )
     item_dump( wais[i].prefix );
 } else out<< "<getcant/>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cwwwstatmod::run() {
  cgicc::const_form_iterator fi = cgi.getElement("id");
  if( fi != env.cgi_end_get() && fi->getValue() == _id ) {
    idb.reset(cinfodb::alloc());

    domains = "";
    empty = true;
    out<< "<mod><wwwstat sid=\"" << env.sess_get().getid() <<"\">";
    
    if ( sessb.ai.flags & cvq::admin ) act();
    else act_user();
    
    if( empty ) out<< "<noent/>";
    out<< "</wwwstat></mod>";
    env.sess_get().setval("stat_domains", domains);
    return done;
  }
  return notme;
}
