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

#include <unistd.h>
#include <iostream>
#include <new>
#include <string>
#include <stdexcept>
#include <cgicc/Cgicc.h>
#include <deque>
#include <fstream>
#include <errno.h>
#include <algorithm>
#include <sstream>

#include <pg_config.h>
#include <libpq++.h>

#include <vq_init.h>
#include <util.h>
#include <sig.h>
#include <valid.h>
#include <lower.h>

#include "cdbdaemon.h"
#include "cgi.h"
#include "conf_pass_ok.h"
#include "conf_user_ok.h"
#include "conf_html_br.h"
#include "conf_pgsql.h"
#include "conf_notify.h"
#include "html.h"

using namespace std;
using namespace cgicc;

const char *me= NULL;

Cgicc *cgi=NULL;
const_form_iterator cgie;
const CgiEnvironment *ce = NULL;

ostream *o=NULL, *e=NULL;

cdbdaemon *db = NULL;

string locale; // localization information (for example pl_PL)
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
void start( int ac, char **av, ostream & o, ostream & e ) 
  throw(cdbdaemon::rt_error,bad_alloc,runtime_error)
{
    // setup
    me = *av;
    ::o = &o;
    ::e = &e;
    
    if(!sig_pipeign()) throw runtime_error("can't setup signal handler for SIGPIPE");
            
    cgi = new Cgicc();
    cgie = cgi->getElements().end();
    ce = & cgi->getEnvironment();
    
    o<<"<vqrescue>";
    cgi_var(cgi, "email", email );
    cgi_var(cgi, "locale", locale);
    if(locale.empty()) locale = "pl_PL";
    if( stringsAreEqual(ce->getRequestMethod(),"POST") ) {
      if( ! email.empty() ) {
        string::size_type atpos = email.find('@');

        if( atpos != string::npos && ! (ui.login=email.substr(0, atpos)).empty()
            && ! (di.name=email.substr(atpos+1)).empty() ) {
          db_init(&db);
          // get needed stuff from db
          db->locale_set(locale);
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
