/**
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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

#include "cdbdaemon.h"
#include "cgi.h"
#include "freemail_conf.h"
#include "html.h"
#include "sabmain.h"

using namespace std;
using namespace cgicc;

const char *me= NULL;

Cgicc *cgi=NULL;
const_form_iterator cgie;
const CgiEnvironment *ce = NULL;

ostream *o=NULL, *e=NULL;

cdbdaemon *db = NULL;

string localeinfo; // localization information (for example pl_PL)
string email;
cdbdaemon::user_info ui;
cdbdaemon::domain_info di;

/**
*/
bool post_act() {
 string pass, step;
 // check data with the posted one, may be we don't have to select all this stuff
 cgi_var(cgi, "area", ui.area_id);
 cgi_var(cgi, "work", ui.work_id);
 cgi_var(cgi, "edu", ui.edu_id);
 cgi_var(cgi, "sex", ui.sex_id);
 cgi_var(cgi, "country", ui.country);
 cgi_var(cgi, "city", ui.city);
 cgi_var(cgi, "birthday", ui.birthday);
 cgi_var(cgi, "re_ans", ui.re_ans);
#define spec_dec(x) x = html_spec_dec(x)
 spec_dec(ui.country);
 spec_dec(ui.city);
 spec_dec(ui.birthday);
 spec_dec(ui.re_ans);
#undef spec_dec

 if( ! ui.area_id.empty() && ! ui.work_id.empty() 
     && ! ui.edu_id.empty() && ! ui.sex_id.empty()
     && ! ui.country.empty() && ! ui.city.empty()
     && ! ui.birthday.empty() && ! ui.re_ans.empty()
     && db->ui_cmp_by_name(ui) == cdbdaemon::err_no 
     && db->user_pass_get(di.name, ui.login, pass) == cdbdaemon::err_no ) {
   *o<<"<step2 pass=\""<< pass <<"\"/>";
   return true;
 }

 // select works, edus, etc. and let user choose it
 vector<cdbdaemon::domain_info> domains;
 vector<cdbdaemon::edu_info> edu_levs;
 vector<cdbdaemon::work_info> works;
 vector<cdbdaemon::area_info> areas;
 vector<cdbdaemon::sex_info> sexes;

 if( db->edu_ls(edu_levs) != cdbdaemon::err_no
     || db->work_ls(works) != cdbdaemon::err_no
     || db->area_ls(areas) != cdbdaemon::err_no
     || db->sex_ls(sexes) != cdbdaemon::err_no
     || db->ui_re_que_get_by_name(di.id, ui.login, ui.re_que) != cdbdaemon::err_no ) return false;
 *o<<"<step1>";
 
 if( ! cgi_var(cgi, "step", step) || step != "0" ) {
   *o<<"<matchno/>"; 
 }
         
 *o<<"<email val=\"" << email << "\"/>";
 *o<<"<re_que val=\"" << ui.re_que << "\"/>";
 *o<<"<edu>";
 for( vector<cdbdaemon::edu_info>::const_iterator beg = edu_levs.begin(), 
      end=edu_levs.end(); beg!=end; ++beg ) {
         *o<<"<lev id=\"" << beg->id <<"\" name=\"" << beg->name <<"\"";
         if( ui.edu_id == beg->id ) *o<<" sel=\"1\" ";
         *o<<"/>";
 }
 *o<<"</edu>";
 *o<<"<works>";
 for( vector<cdbdaemon::work_info>::const_iterator beg = works.begin(), 
      end=works.end(); beg!=end; ++beg ) {
         *o<<"<work id=\"" << beg->id <<"\" name=\"" << beg->name <<"\"";
         if( ui.work_id == beg->id ) *o<<" sel=\"1\" ";
         *o<<"/>";
 }
 *o<<"</works>";
 *o<<"<areas>";
 for( vector<cdbdaemon::area_info>::const_iterator beg=areas.begin(),
      end=areas.end(); beg!=end; ++beg ) {
         *o<<"<area id=\""<< beg->id <<"\" name=\""<< beg->name <<"\"";
         if( ui.area_id == beg->id ) *o<<" sel=\"1\" ";
         *o<<"/>";
 }
 *o<<"</areas>";
 *o<<"<sexes>";
 for( vector<cdbdaemon::sex_info>::const_iterator beg=sexes.begin(),
      end=sexes.end(); beg!=end; ++beg ) {
         *o<<"<sex id=\"" << beg->id <<"\" name=\""<< beg->name <<"\"";
         if( ui.sex_id == beg->id ) *o<<" sel=\"1\" ";
         *o<<"/>";
 }
 *o<<"</sexes>";
#define spec_enc(x,y) *o<<"<" x " val=\"" << html_spec_enc(y) << "\"/>"
 spec_enc("country", ui.country);
 spec_enc("city", ui.city);
 spec_enc("birthday", ui.birthday);
 spec_enc("re_ans", ui.re_ans);
#undef spec_enc

 *o << "</step1>";
 return true;
}

/**
 *
 */
void start( int ac, char **av, ostream & o, ostream & e, xslt_args_type & ) {
    // setup
    me = *av;
    ::o = &o;
    ::e = &e;
    
    if(!sig_pipe_ign()) throw runtime_error("can't setup signal handler for SIGPIPE");
            
    cgi = new Cgicc();
    cgie = cgi->getElements().end();
    ce = & cgi->getEnvironment();
    
    o<<"<vqrescue>";
    cgi_var(cgi, "email", email );
    cgi_var(cgi, "locale", localeinfo);
    if(localeinfo.empty()) localeinfo = "pl_PL";
    if( stringsAreEqual(ce->getRequestMethod(),"POST") ) {
      if( ! email.empty() ) {
        string::size_type atpos = email.find('@');

        if( atpos != string::npos && ! (ui.login=email.substr(0, atpos)).empty()
            && ! (di.name=email.substr(atpos+1)).empty() ) {
          db = cdbdaemon::alloc();
          // get needed stuff from db
          db->locale_set(localeinfo);
          if( db->domain_get_by_name(di) == cdbdaemon::err_no ) {
            ui.dom_id = di.id;
            if( post_act() ) goto start_end;
          }
        }
      }
      o<<"<inv><emailno/></inv>";
    }
start_end:
    o<<"</vqrescue>";
}
