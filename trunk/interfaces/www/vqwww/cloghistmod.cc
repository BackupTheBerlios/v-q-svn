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
#include <memory>
#include <sstream>

#include "cloghistmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "vqwww_conf.h"
#include "html.h"

using namespace std;
using namespace vqwww;
using namespace cgicc;

const char cloghistmod::_id[] = "loghist";
const char cloghistmod::_menu[] = "<loghist/>";

/**
 * Try to get user supplied start and count
 * \param start will be set to value passed by GET (not chagned if there's no such variable)
 * \param cnt will be set to value passed in GET request (not changed if there's no such variable)
 */
void cloghistmod::range_get( clogger::size_type & start, clogger::size_type & cnt ) {
	const_form_iterator get_start(cgi.getElement("start"));
	const_form_iterator get_cnt(cgi.getElement("cnt"));
	istringstream in;
	clogger::size_type tmp;
	
	if( get_start != env.cgi_end_get() ) {
			in.str(get_start->getValue());
			in>>tmp;
			if( ! in.bad() && ! in.fail() && tmp > 0 ) 
					start = tmp;
	}
	if( get_cnt != env.cgi_end_get() ) {
			in.clear();
			in.str(get_cnt->getValue());
			in>>tmp;
			if( ! in.bad() && ! in.fail() && tmp > 0 ) 
					cnt = tmp;
	}
}

/**
 * \param cnt Number of all entries
 * \param sh_start Start printing from this element
 * \param sh_cnt Print only this number of elements.
 */
void cloghistmod::navi_dump( clogger::size_type cnt, 
		clogger::size_type sh_start, clogger::size_type sh_cnt ) {

	clogger::size_type page=sh_start/sh_cnt, page_end = cnt/sh_cnt+(cnt%sh_cnt ? 1 : 0);
	clogger::size_type start=page-(page%10), end=page_end < start+10 ? page_end : start+10;
	
	ostringstream navi;
	bool show = false;
	
	navi<<"<navi cnt=\""<< sh_cnt <<"\">";
	if( cnt > sh_cnt ) {
			show = true;
			navi<<"<start/>";
			navi<<"<end start=\""<< (sh_cnt == 1 ? cnt-1 : cnt-(cnt%sh_cnt ? cnt%sh_cnt : sh_cnt)) <<"\"/>";
	}
	if( start > 0 ) {
			navi<<"<prev start=\"" << (start-1)*sh_cnt <<"\"/>";
			show = true;
	}
	
	if( end > 0 && page_end > end ) {
			navi<<"<next start=\"" << (end > 0 ? end*sh_cnt : 0) <<"\"/>";
			show = true;
	}

	for( ; start<end && end>1; ++start ) {
			navi<<"<page";
			if( start==page ) navi<<" cur=\"1\" ";
			navi<<" start=\""<< start*sh_cnt <<"\""
				<<" num=\""<< start+1 << "\"/>";
	}
	navi<<"</navi>";

	if( show ) out<<navi.str();
}

/**
 *
 */
void cloghistmod::act( clogger * log ) {
    out<< "<mod><loghist>";
	typedef vector<clogger::entry_type> hist_type;
	hist_type hist;

	log->domain_set(sessb.ai.dom);
	log->login_set(sessb.ai.user);

	clogger::size_type cnt;

	if( log->dom_log_cnt_log(cnt) == clogger::err_no ) {
			clogger::size_type sh_start = 0, sh_cnt = 20;
			range_get(sh_start, sh_cnt);

			navi_dump(cnt, sh_start, sh_cnt);
	
			hist_type::const_iterator beg, end;
			switch( log->dom_log_read_log(hist, sh_start, sh_cnt) ) {
			case clogger::err_no:
					for( beg=hist.begin(), end=hist.end(); beg!=end; ++beg ) {
							out<<"<login>"
								<<"<time>"<<beg->time<<"</time>"
								<<"<ser>"<<static_cast<int>(beg->ser)<<"</ser>"
								<<"<msg>"<<beg->msg<<"</msg>"
								<<"<ip>"<<beg->ip<<"</ip>"
								<<"<res>"<<static_cast<int>(beg->res)<<"</res>"
								<<"</login>";
					}
					break;
			case clogger::err_empty:
					out<<"<empty/>";
					break;
			default:
					out<<"<getcant/>";
					err<<__FILE__<<": "<<__LINE__<<": "<<log->err_report()<<endl;
			}
	} else {
			out<<"<getcant/>";
			err<<__FILE__<<": "<<__LINE__<<": "<<log->err_report()<<endl;
	}
	
	out<< "</loghist></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cloghistmod::run() {
    const_form_iterator id = cgi.getElement("id");
    if( id != env.cgi_end_get() && id->getValue() == _id ) {
			act(&env.logger_get());
			return done;
    }
    return notme;
}
