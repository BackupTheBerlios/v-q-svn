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
#include <sstream>
#include <cgicc/Cgicc.h>
#include <vqwww.h>
#include <html.h>
#include <cgi.h>
#include <valid.h>

#include "cuimod.h"
#include "freemail_conf.h"

using namespace std;
using namespace cgicc;
using namespace freemail;
using namespace valid;

const char cuimod::_id[] = "ui";
const char cuimod::_conf[] = "<ui/><desc><ui/></desc>";
const char cuimod::_name[] = "";

/**
 *
 */
cuimod::cuimod() {
 modinfo = mi_conf;
}

/**
 *
 */ 
void cuimod::form2ints() {
  ostringstream int_name;
  string inter;
  vector<cdbdaemon::int_info>::size_type i, cnt;
  vector<cdbdaemon::int_info>::const_iterator icur, iend = ints.end();
  ui.ints_id.clear();
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
 *
 */
void cuimod::ints2form() {
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
*/
void cuimod::post_act() {
#define ok_not(x) do { \
        *o<<x; \
        ok = false; \
} while(0)
  bool ok = true;
  string::size_type len;

  *o<<"<inv>";
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
  
  *o<<"<ints ";
  if( ! ui.ints_id.size() ) ok_not("no=\"1\" ");
  *o<<"/>";
  *o<<"</inv>";

  if( ok ) {
    if( idb->ui_chg(ui) != cdbdaemon::err_no ) {
      *e<<": idb->ui_chg: "<< idb->err() << endl;
      *o<<"<chgcant/>";
    }
  }
#undef ok_not
}

/**
 *
 */
void cuimod::act() {
  *o<<"<mod><ui>";
  dominfo.name = sessv.ai.dom;
  if( idb->domain_get_by_name(dominfo) != cdbdaemon::err_no ) {
    *o << "<getcant/>";
    *e << "can't get domain info: " << idb->err() << endl;
    goto cuimod_act_end;
  }
  
  ui_org.login = sessv.ai.user;
  ui_org.dom_id = dominfo.id;
  if( idb->ui_get_by_name(ui_org) != cdbdaemon::err_no ) {
    *o << "<getcant/>";
    *e << "can't get user info: " << idb->err() << endl;
    goto cuimod_act_end;
  }

  idb->locale_set(ui_org.locale);
  if( idb->edu_ls(edu_levs) != cdbdaemon::err_no
      || idb->work_ls(works) != cdbdaemon::err_no
      || idb->int_ls(ints) != cdbdaemon::err_no
      || idb->area_ls(areas) != cdbdaemon::err_no
      || idb->sex_ls(sexes) != cdbdaemon::err_no ) {
    *o << "<getcant/>";
    *e << "can't get info: " << idb->err() << endl;
    goto cuimod_act_end;
  }

  ui = ui_org;
  if( reqmet == post ) {
    // real action 
    cgi_var(cgi, "country", ui.country);
    cgi_var(cgi, "area", ui.area_id);
    cgi_var(cgi, "city", ui.city);
    cgi_var(cgi, "sex", ui.sex_id);
    cgi_var(cgi, "edu", ui.edu_id);
    cgi_var(cgi, "work", ui.work_id);
    cgi_var(cgi, "birthday", ui.birthday);
#define spec_dec(x) x = html_spec_dec(x)
    spec_dec(ui.country);
    spec_dec(ui.city);
    spec_dec(ui.birthday);
#undef spec_dec
    form2ints();

    post_act();
  }

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
  ints2form();
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

  *o<<"<country val=\"" << html_spec_enc(ui.country) << "\" minl=\""<< ac_cntry_minl.val_str()<<"\"/>";
  *o<<"<city val=\"" << html_spec_enc(ui.city) << "\" minl=\""<< ac_city_minl.val_str() <<"\"/>";
  *o<<"<birthday val=\"" << html_spec_enc(ui.birthday) << "\"/>";

cuimod_act_end:
  *o<<"</ui></mod>";
}

/**
 *
 */
char cuimod::run() {
 try {
   idb.reset(new cdbdaemon());
 } catch(const exception &e) {
   *::e<<"exception: "<<e.what()<<endl;
   return notme;
 } catch(...) {
   *::e<<"unexpected exception"<<endl;
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
