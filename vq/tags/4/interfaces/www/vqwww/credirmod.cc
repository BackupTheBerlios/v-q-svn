/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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

#include "credirmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "vqwww_conf.h"

using namespace std;
using namespace vqwww;
using namespace cgicc;

const char credirmod::_id[] = "redir";
const char credirmod::_conf[] = "<redir/><desc><redir/></desc>";

void credirmod::rmspace(string &s) {
	string::iterator ptr=s.begin(), end=s.end();
	while( ptr != end ) {
			if( isspace(*ptr) ) {
					s.erase(ptr);
					end = s.end();
			} else ptr ++;
	}
}

/**
 *
 */
void credirmod::post_read( vector<cvq::udot_info>::size_type & cnt,
		vector<cvq::udot_info> &torep, 
		vector<cvq::udot_info> &toadd, 
		vector<cvq::udot_info> &torm ) {

	string val, id;
	ostringstream os;
	cvq::udot_info uitmp;
	vector<cvq::udot_info>::size_type i;
	vector<cvq::udot_info>::iterator ptr;

	uitmp.type = cvq::redir;

	for( i=1; i <= cnt; i++ ) {
			os.str("");
			os<<i;
								
			if( ! cgi_var(env.cgi_get(), "item"+os.str(), val) )
					continue;
	
			rmspace(val);
			if( val.empty() ) continue;
			uitmp.val = val;
		
			cgi_var(env.cgi_get(), "itemid"+os.str(), id);
			rmspace(id);
			
			// if item has valid id. we're goint to replace it, otherwise add
			if( ! id.empty() ) {
					cmp.id = id;
					
					// look for element with specified id.
					ptr=find_if(torm.begin(), torm.end(), cmp);
					
					if( ptr != torm.end() ) {
							if ( ptr->val != val ) {
									uitmp.id = id;
									torep.push_back(uitmp);
							}
							torm.erase(ptr);
							continue;
					}
			}
			uitmp.id.erase();
			toadd.push_back(uitmp);
	}
}

/**
 *
 */
void credirmod::post( vector<cvq::udot_info> & uis,
		vector<cvq::udot_info>::size_type & cnt ) {
	string val, id;
	vector<cvq::udot_info> torep, toadd, torm(uis);
	vector<cvq::udot_info>::iterator ptr, cur, end;
			
	if( ac_redir_max.val_int() > cnt ) 
			cnt = ac_redir_max.val_int();
			
	// read variables passed from POST request
	post_read( cnt, torep, toadd, torm ); 

	out<<"<inv>";

	const cvq::auth_info &ai(env.sessb_get().ai);
	cvq & vq(env.vq_get());

	for( cur=toadd.begin(), end=toadd.end(); cur!=end; ++cur ) {
			if(vq.udot_add(ai.dom, ai.user, "", *cur)) {
					err<<"credirmod::udot_add: "<<vq.err_report()<<endl;
					out<<"<add>"<< cur->val <<"</add>";
			}
			if(vq.udot_rm(ai.dom, ai.user, "", cvq::maildir) ) {
					err<<"credirmod::udot_rm: "<<vq.err_report()<<endl;
			}
			uis.push_back(*cur);
	}
	for( cur=torep.begin(), end=torep.end(); cur!=end; ++cur ) {
			if(vq.udot_rep(ai.dom, ai.user, "", *cur)) {
					err<<"credirmod::udot_rep: "<<vq.err_report()<<endl;
					out<< "<rep>"<< cur->val <<"</rep>";
			}
			cmp.id = cur->id;
			ptr = find_if(uis.begin(), uis.end(), cmp);
			if( ptr != uis.end() ) *ptr = *cur;
	}
	for( cur=torm.begin(), end=torm.end(); cur!=end; ++cur ) {
			cvq::size_type udot_cnt;
			if( vq.udot_type_cnt(ai.dom, ai.user, "", cvq::redir, 
						udot_cnt ) ) {
					err<<"credirmod::udot_type_cnt: "<<vq.err_report()<<endl;
					continue;
			}
					
			if( udot_cnt <= 1
				&& vq.udot_has(ai.dom, ai.user, "", 
						cvq::maildir ) == cvq::err_noent ) {
					switch( vq.udot_add_md_def(ai.dom, ai.user, "") ) {
					case cvq::err_exists:
					case cvq::err_no:
							break;
					default: 
							err<<"credirmod::udot_add_md_def: "<<vq.err_report()<<endl;
							continue;
					}
			}
			
			if( vq.udot_rm(ai.dom, ai.user, "", cur->id)) {
					err<<"credirmod::udot_rm: "<<vq.err_report()<<endl;
					out<< "<rm>"<<cur->val<<"</rm>";
					continue;
			}
			cmp.id = cur->id;
			ptr = find_if(uis.begin(), uis.end(), cmp);
			if( ptr != uis.end() ) uis.erase(ptr);
	}
	out<<"</inv>";
}
/**
 *
 */
void credirmod::list( vector<cvq::udot_info> & uis ) {
	vector<cvq::udot_info>::size_type cnt = uis.size(), i;
	if( env.cgi_rm_get() == cenvironment::rm_post ) {
			post(uis, cnt);
	}

	for( i=0, cnt = uis.size(); i<cnt; ++i )
			out <<"<item val=\"" << uis[i].val << "\" id=\""
				<< uis[i].id << "\"/>";
			
	if( cnt < ac_redir_max.val_int() ) {
			cnt = ac_redir_max.val_int() - cnt;
			for( i=0; i < cnt; ++i )
					out<<"<item/>";
	}
}
/***************************************************************************
 * 
 * *************************************************************************/
void credirmod::act() {
	vector<cvq::udot_info> uis;
	out<< "<mod><redir>";
	if(! env.vq_get().udot_ls(env.sessb_get().ai.dom, env.sessb_get().ai.user, "", 
				cvq::redir, uis ) ) {
			list( uis );
	} else 
			out<< "<getcant/>";
	out<< "</redir></mod>";
}
/**************************************************************************
 *  
 * ***********************************************************************/
char credirmod::run() {
	const_form_iterator fi = env.cgi_get().getElement("conf");
	if( fi != env.cgi_end_get() ) {
			fi = env.cgi_get().getElement("id");
			if( fi != env.cgi_end_get() && fi->getValue() == _id ) {
					if( env.vq_get().udot_sup_is(cvq::redir) 
						&& env.vq_get().udot_sup_is(cvq::maildir) ) {
							act();
							return done;
					}
			}
	}
	return notme;
}
