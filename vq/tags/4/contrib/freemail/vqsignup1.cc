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

#include <pqxx/all.h>

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
using namespace freemail;
using namespace valid;

const char *me= NULL;

Cgicc *cgi=NULL;
const_form_iterator cgie;
const CgiEnvironment *ce = NULL;

ostream *o=NULL, *e=NULL;

cdbdaemon *db = NULL;

string localeinfo; // localization information (for example pl_PL)
vector<cdbdaemon::domain_info> domains;
vector<cdbdaemon::edu_info> edu_levs;
vector<cdbdaemon::work_info> works;
vector<cdbdaemon::int_info> ints;
vector<cdbdaemon::area_info> areas;
vector<cdbdaemon::sex_info> sexes;

cdbdaemon::user_info ui;
string pass, pass1;
const char *iid=NULL; // id from users_info

/**
 */
void notify_run() {
    char *av[] = { 
            (char *)ac_notify.val_str().c_str(), 
            (char *)ui.dom_id.c_str(), 
            (char *)ui.login.c_str(), (char *)iid, NULL };
    switch(vfork()) {
    case -1:
      *e<<me<<": vfork failed: "<<strerror(errno)<<endl;
      break;
    case 0:
      execvp( *av, av );
      *e<<me<<": execvp: "<< ac_notify.val_str() <<": "<<strerror(errno)<<endl;
      _exit(1);
    }
}
/**
 *
 */ 
void form2ints() {
  ostringstream int_name;
  string inter;
  vector<cdbdaemon::int_info>::size_type i, cnt;
  vector<cdbdaemon::int_info>::const_iterator icur, iend = ints.end();
  for( i=1, cnt=ints.size(); i<=cnt; ++i ) {
    int_name.str("");
    int_name<<i;
    cgi_var(cgi, "int"+int_name.str(), inter);
    if( ! inter.empty() ) {
      for( icur=ints.begin(); icur!=iend; ++icur ) {
        if( icur->id == inter ) break;
      }
      if( icur != iend ) {
        ui.ints_id.push_back(inter);
      }
    }
  }
}

/**
*/
bool post_act() {
#define ok_not(x) do { \
        *o<<x; \
        ok = false; \
} while(0)
    bool ok = true;
    string::size_type len;

    *o<<"<inv><user ";
    len = ui.login.length();
    if(len<ac_user_minl.val_int()) ok_not("sh=\"1\" ");
    else if(len>ac_user_maxl.val_int()) ok_not("lo=\"1\" ");
    
    if(htmlbad(ui.login, ac_user_ok.val_str().c_str(), ac_html_br.val_str().at(0))) 
            ok_not("dirty=\"1\" ");
    *o<<"/>";
    *o<<"<pass ";
    len = pass.length();
    if(len<ac_pass_minl.val_int()) ok_not("sh=\"1\" ");
    else if(len>ac_pass_maxl.val_int()) ok_not("lo=\"1\" ");
    if(htmlbad(pass, ac_pass_ok.val_str().c_str(), ac_html_br.val_str().at(0)))
            ok_not("dirty=\"1\" ");
    if( pass != pass1 ) ok_not(" nomatch=\"1\" ");
    *o<<"/>";
    // country
    *o<<"<country ";
    len = ui.country.length();
    if(len <= ac_cntry_minl.val_int()) ok_not("sh=\"1\" ");
    *o<<"/>";
    // city
    *o<<"<city ";
    len = ui.city.length();
    if(len <= ac_city_minl.val_int()) ok_not("sh=\"1\" ");
    *o<<"/>";
    // birthday
    *o<<"<birthday ";
    string t;
    if( ! (t=date_valid(ui.birthday)).empty() ) ok_not("inv=\"1\" ");
    *o<<"/>";
    
    form2ints();
    *o<<"<ints ";
    if( ! ui.ints_id.size() ) ok_not("no=\"1\" ");
    *o<<"/>";
    *o<<"</inv>";
    
    if( ok ) {
      auto_ptr<cvq> vq(cvq::alloc());
      int ret;

      cdbdaemon::domain_info di;
      
      di.id = ui.dom_id;
      if( db->domain_get(di) == cdbdaemon::err_no 
          && db->ui_add(ui) == cdbdaemon::err_no ) {
        if( (ret=vq->user_add(ui.login, di.name, pass)) ) {
          db->ui_rm(ui.id);
          *e<<me<<": vq->user_add: "<<vq->err_report()<<endl;
        } else {
          *o<<"<add ok=\"\">";
          *o<<"<domain>"<< di.name <<"</domain>";
          *o<<"<user>" << ui.login << "</user>";
          *o<<"</add>";
          if(! ac_notify.val_str().empty()) notify_run();
          return true;
        }
      } else {
        *e<<me<<": db->domain_get,ui_add: "<< db->err() << endl;
      }
      *o<<"<add user_ex=\"\"/>";
    }
    return false;
#undef ok_not
}

void ints2form() {
  vector<cdbdaemon::int_info>::const_iterator beg=ints.begin(), end=ints.end();
  vector<string>::const_iterator icur, iend = ui.ints_id.end();
  *o<<"<ints>";
  for( ; beg!=end; ++beg ) {
    *o<<"<int id=\"" << beg->id <<"\" name=\"" << beg->name <<"\"";
    for( icur=ui.ints_id.begin(); icur!=iend; ++icur ) {
      if( *icur == beg->id ) { 
        *o<<" sel=\"1\" ";
        break;
      }
    }
    *o<<"/>";
  }
  *o<<"</ints>";
}

/**
 *
 */
void start( int ac, char **av, ostream & o, ostream & e, xslt_args_type & ) {
    // setup
    me = *av;
    ::o = &o;
    ::e = &e;
   
    if(!sig_chld_nocldwait()) throw runtime_error("can't setup signal handler for SIGCHLD");
    if(!sig_pipeign()) throw runtime_error("can't setup signal handler for SIGPIPE");
            
    cgi = new Cgicc();
    cgie = cgi->getElements().end();
    ce = & cgi->getEnvironment();
    
    o<<"<vqsignup>";
 
    db = cdbdaemon::alloc();
    
    // get needed stuff from db
    cgi_var(cgi, "locale", localeinfo);
    if(localeinfo.empty()) localeinfo = "pl_PL";
    db->locale_set(localeinfo);
    if( db->edu_ls(edu_levs) != cdbdaemon::err_no 
        || db->work_ls(works) != cdbdaemon::err_no
        || db->int_ls(ints) != cdbdaemon::err_no
        || db->area_ls(areas) != cdbdaemon::err_no
        || db->sex_ls(sexes) != cdbdaemon::err_no
        || db->domain_ls(domains) != cdbdaemon::err_no ) {
      o << "<getcant/>";
      goto start_end;
    }
    if( sexes.size() != 2 )
      throw runtime_error("sexes.size()!=2");
    
    // real action
    cgi_var(cgi, "user", ui.login); ui.login=lower(ui.login);
    cgi_var(cgi, "domain", ui.dom_id );
    cgi_var(cgi, "pass", pass);
    cgi_var(cgi, "pass1", pass1);
    cgi_var(cgi, "country", ui.country);
    cgi_var(cgi, "area", ui.area_id);
    cgi_var(cgi, "city", ui.city);
    cgi_var(cgi, "sex", ui.sex_id);
    cgi_var(cgi, "edu", ui.edu_id);
    cgi_var(cgi, "work", ui.work_id);
    cgi_var(cgi, "birthday", ui.birthday);
    cgi_var(cgi, "re_que", ui.re_que);
    cgi_var(cgi, "re_ans", ui.re_ans);
#define spec_dec(x) x = html_spec_dec(x)
    spec_dec(ui.country);
    spec_dec(ui.city);
    spec_dec(ui.birthday);
    spec_dec(ui.re_que);
    spec_dec(ui.re_ans);
#undef spec_dec

    if( stringsAreEqual(ce->getRequestMethod(),"POST") ) {
            if( post_act() ) goto start_end;
    }
    o<<"<edu>";
    for( vector<cdbdaemon::edu_info>::const_iterator beg = edu_levs.begin(), 
         end=edu_levs.end(); beg!=end; ++beg ) {
            o<<"<lev id=\"" << beg->id <<"\" name=\"" << beg->name <<"\"";
            if( ui.edu_id == beg->id ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</edu>";
    o<<"<works>";
    for( vector<cdbdaemon::work_info>::const_iterator beg = works.begin(), 
         end=works.end(); beg!=end; ++beg ) {
            o<<"<work id=\"" << beg->id <<"\" name=\"" << beg->name <<"\"";
            if( ui.work_id == beg->id ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</works>";
    ints2form();
    o<<"<areas>";
    for( vector<cdbdaemon::area_info>::const_iterator beg=areas.begin(),
         end=areas.end(); beg!=end; ++beg ) {
            o<<"<area id=\""<< beg->id <<"\" name=\""<< beg->name <<"\"";
            if( ui.area_id == beg->id ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</areas>";
    o<<"<sexes>";
    for( vector<cdbdaemon::sex_info>::const_iterator beg=sexes.begin(),
         end=sexes.end(); beg!=end; ++beg ) {
            o<<"<sex id=\"" << beg->id <<"\" name=\""<< beg->name <<"\"";
            if( ui.sex_id == beg->id ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</sexes>";
    o<<"<domains>";
    for( vector<cdbdaemon::domain_info>::const_iterator beg=domains.begin(),
         end=domains.end(); beg!=end; ++beg ) {
            o<<"<domain id=\"" << beg->id <<"\" name=\"" << beg->name << "\"";
            if( ui.dom_id==beg->id ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</domains>";
    o<<"<user val=\"" << ui.login << "\" allowed=\"" << ac_user_ok.val_str();
	o<< "\" minl=\""<<ac_user_minl.val_str()<<"\" maxl=\""<<ac_user_maxl.val_str()<<"\"/>";
    o<<"<pass val=\"" << pass << "\" allowed=\"" << ac_pass_ok.val_str()
     << "\" minl=\""<<ac_pass_minl.val_str()<<"\" maxl=\""<<ac_pass_maxl.val_str()<<"\"/>";
    o<<"<pass1 val=\"" << pass1 << "\"/>";
 
    o<<"<country val=\"" << html_spec_enc(ui.country) << "\" minl=\""<< ac_cntry_minl.val_str()<<"\"/>";
    o<<"<city val=\"" << html_spec_enc(ui.city) << "\" minl=\""<< ac_city_minl.val_str() <<"\"/>";

#define spec_enc(x) o<<"<" #x " val=\"" << html_spec_enc(ui.x) << "\"/>"
    spec_enc(birthday);
    spec_enc(re_que);
    spec_enc(re_ans);
#undef spec_enc

start_end:
    o<<"</vqsignup>";
}
