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

/**
 *
 */
cuimod::cuimod( cenvironment & env ) : cmod(env) {
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
	out<<"<ints>";
	for( ; beg!=end; ++beg ) {
			out<<"<int id=\"" << beg->id <<"\" name=\"" << beg->name <<"\"";
			for( icur=ui.ints_id.begin(); icur!=iend; ++icur ) {
					if( *icur == beg->id ) { 
							out<<" sel=\"1\" ";
							break;
					}
			}
			out<<"/>";
	}
	out<<"</ints>";
}

/**
*/
void cuimod::post_act() {
#define ok_not(x) do { \
        out<<x; \
        ok = false; \
} while(0)
	bool ok = true;
	string::size_type len;
	
	out<<"<inv>";
	// country
	out<<"<country ";
	len = ui.country.length();
	if(len <= ac_cntry_minl.val_int()) ok_not("sh=\"1\" ");
	out<<"/>";
	// city
	out<<"<city ";
	len = ui.city.length();
	if(len <= ac_city_minl.val_int()) ok_not("sh=\"1\" ");
	out<<"/>";
	// birthday
	out<<"<birthday ";
	string t;
	if( ! (t=date_valid(ui.birthday)).empty() ) ok_not("inv=\"1\" ");
	out<<"/>";
  
	out<<"<ints ";
	if( ! ui.ints_id.size() ) ok_not("no=\"1\" ");
	out<<"/>";
	out<<"</inv>";

	if( ok ) {
			if( idb->ui_chg(ui) != cdbdaemon::err_no ) {
					err<< *env.av_get() << ": idb->ui_chg: "<< idb->err() << endl;
					out<<"<chgcant/>";
			}
	}

#undef ok_not
}

/**
 *
 */
void cuimod::act() {
	out<<"<mod><ui>";
	dominfo.name = sessb.ai.dom;
	if( idb->domain_get_by_name(dominfo) != cdbdaemon::err_no ) {
			out<< "<getcant/>";
			err<< *env.av_get() << ": can't get domain info: " << idb->err() << endl;
			goto cuimod_act_end;
	}

	ui_org.login = sessb.ai.user;
	ui_org.dom_id = dominfo.id;
	if( idb->ui_get_by_name(ui_org) != cdbdaemon::err_no ) {
			out<< "<getcant/>";
			err<< *env.av_get() << ": can't get user info: " << idb->err() << endl;
			goto cuimod_act_end;
	}

	idb->locale_set(ui_org.locale);
	if( idb->edu_ls(edu_levs) != cdbdaemon::err_no
		|| idb->work_ls(works) != cdbdaemon::err_no
		|| idb->int_ls(ints) != cdbdaemon::err_no
		|| idb->area_ls(areas) != cdbdaemon::err_no
		|| idb->sex_ls(sexes) != cdbdaemon::err_no ) {
			out<< "<getcant/>";
			err<< *env.av_get() << ": can't get info: " << idb->err() << endl;
			goto cuimod_act_end;
	}
	ui = ui_org;
	if( env.cgi_rm_get() == cenvironment::rm_post ) {
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

	out<<"<edu>";
	for( vector<cdbdaemon::edu_info>::const_iterator beg = edu_levs.begin(), 
					end=edu_levs.end(); beg!=end; ++beg ) {
			out<<"<lev id=\"" << beg->id <<"\" name=\"" << beg->name <<"\"";
			if( ui.edu_id == beg->id ) out<<" sel=\"1\" ";
			out<<"/>";
	}

	out<<"</edu>";
	out<<"<works>";

	for( vector<cdbdaemon::work_info>::const_iterator beg = works.begin(), 
					end=works.end(); beg!=end; ++beg ) {
			out<<"<work id=\"" << beg->id <<"\" name=\"" << beg->name <<"\"";
			if( ui.work_id == beg->id ) out<<" sel=\"1\" ";
			out<<"/>";
	}
	out<<"</works>";
	ints2form();
	out<<"<areas>";
	for( vector<cdbdaemon::area_info>::const_iterator beg=areas.begin(),
					end=areas.end(); beg!=end; ++beg ) {
			out<<"<area id=\""<< beg->id <<"\" name=\""<< beg->name <<"\"";
			if( ui.area_id == beg->id ) out<<" sel=\"1\" ";
			out<<"/>";
	}
	out<<"</areas>";
	out<<"<sexes>";

	for( vector<cdbdaemon::sex_info>::const_iterator beg=sexes.begin(),
					end=sexes.end(); beg!=end; ++beg ) {
			out<<"<sex id=\"" << beg->id <<"\" name=\""<< beg->name <<"\"";
			if( ui.sex_id == beg->id ) out<<" sel=\"1\" ";
			out<<"/>";
	}
	out<<"</sexes>";

	out<<"<country val=\"" << html_spec_enc(ui.country) << "\" minl=\""<< ac_cntry_minl.val_str()<<"\"/>";
	out<<"<city val=\"" << html_spec_enc(ui.city) << "\" minl=\""<< ac_city_minl.val_str() <<"\"/>";
	out<<"<birthday val=\"" << html_spec_enc(ui.birthday) << "\"/>";

cuimod_act_end:
	out<<"</ui></mod>";
}

/**
 *
 */
char cuimod::run() {
	const_form_iterator fi = cgi.getElement("conf");
	if( fi != env.cgi_end_get() ) {
			fi = cgi.getElement("id");
			if( fi != env.cgi_end_get() && fi->getValue() == _id ) {
					idb.reset(new cdbdaemon());

					act();
					return done;
			}
	}
	return notme;
}
