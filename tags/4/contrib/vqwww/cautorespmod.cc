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

#include <new>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <cerrno>

#include <iconv.h>

#include <sys.h>

#include "cautorespmod.h"
#include "vqwww_conf.h"
#include "vqwww.h"
#include "cgi.h"
#include "html.h"

using namespace std;
using namespace vqwww;
using namespace cgicc;

const char cautorespmod::_id[] = "autoresp";
const char cautorespmod::_conf[] = "<autoresp/><desc><autoresp/></desc>";

void cautorespmod::change(string &autoresp, vector<cvq::udot_info> &uis ) {
	bool chgcant = false;
	if( autoresp.empty() ) {
			if( vq.udot_has(sessb.ai.dom, sessb.ai.user, "", 
						cvq::maildir) == cvq::err_noent
				 && vq.udot_has(sessb.ai.dom, sessb.ai.user, "", 
						cvq::redir) == cvq::err_noent ) {
					switch( vq.udot_add_md_def(sessb.ai.dom, sessb.ai.user, "") ) {
					case cvq::err_exists:
					case cvq::err_no: break;
					default: 
							err<<"cautorespmod::udot_add_md_def: "<<vq.err_report()<<endl;
							chgcant = true;
					}
			}
			if(!chgcant) {
					if( vq.udot_rm(sessb.ai.dom, sessb.ai.user, 
						 "", cvq::autoresp) ) {
							err<<"cautorespmod::udot_rm: "<<vq.err_report()<<endl;
					} else uis.clear();
			}
	} else {
			cvq::udot_info ui;
			cmapconf::map_type::const_iterator liter;
			liter = ac_codepages.val_map().find(env.locale_get());

			ui.val = liter != ac_codepages.val_map().end() ? liter->second : "UTF-8";
			ui.val += '\0'+convert(ui.val, "UTF-8", autoresp);

			ui.val = to_hex( 
				reinterpret_cast<const unsigned char *>(ui.val.data()), 
				ui.val.length());
			
			if(!uis.size()) {
					if( vq.udot_has(sessb.ai.dom, sessb.ai.user, "", 
								cvq::maildir) == cvq::err_noent
						 && vq.udot_has(sessb.ai.dom, sessb.ai.user, "", 
								cvq::redir) == cvq::err_noent ) {
							switch( vq.udot_add_md_def(sessb.ai.dom, sessb.ai.user, "") ) {
							case cvq::err_exists:
							case cvq::err_no: break;
							default: 
									err<<"cautorespmod::udot_add_md_def: "<<vq.err_report()<<endl;
									chgcant = true;
							}
					}
					if( ! chgcant ) {
							ui.type = cvq::autoresp;
							if(vq.udot_add(sessb.ai.dom, sessb.ai.user, "", ui)) {
									err<<"cautorespmod::udot_add: "<<vq.err_report()<<endl;
									chgcant = true;
							} else {
									uis.push_back(ui);
							}
					}
			} else if(autoresp != uis[0].val) {
					ui.id = uis[0].id;
					ui.type = uis[0].type;
					if(vq.udot_rep(sessb.ai.dom, sessb.ai.user, "", ui)) {
							err<<"cautorespmod::udot_rep: "<<vq.err_report()<<endl;
							chgcant = true;
					} else {
							uis[0].val = ui.val;
					}
			}
	}
	if( chgcant ) out<<"<chgcant/>";
}

/***************************************************************************
 *  
 * *************************************************************************/
void cautorespmod::act() {
	vector<cvq::udot_info> uis;
	out<< "<mod><autoresp>";
	if(! vq.udot_ls(sessb.ai.dom, sessb.ai.user, "", cvq::autoresp, uis ) ) {
			if( env.cgi_rm_get() == cenvironment::rm_post ) {
					bool ok = true;
					string autoresp;
					
					// we get list of items
					cgi_var(cgi, "autoresp", autoresp);
					autoresp = html_spec_dec(autoresp);

					if( ok ) {
							try {
									change(autoresp, uis);
							} catch( const exception & e ) {
									err<<e.what()<<endl;
									out<<"<chgcant/>";
							}
					}
			}
			out<<"<autoresp>";
			if( uis.size() ) {
					string ar(hex_from(uis[0].val));
					string::size_type zero = ar.find('\0');
					if(ar.length() > zero+1)
							out<<html_spec_enc(
								convert("UTF-8", ar.substr(0, zero), 
									ar.substr(zero+1, ar.find('\0', zero+1))) )
								<<endl; 
			}
			out<< "</autoresp>";
	} else out<< "<getcant/>";
	out<< "</autoresp></mod>";
}
/**************************************************************************
 *		
 ************************************************************************/
char cautorespmod::run() {
	const_form_iterator fi = cgi.getElement("conf");
	if( fi != env.cgi_end_get() ) {
			fi = cgi.getElement("id");
			if( fi != env.cgi_end_get() && fi->getValue() == _id ) {
					if( vq.udot_sup_is(cvq::autoresp) 
						&& vq.udot_sup_is(cvq::maildir) ) {
							act();
							return done;
					}
			}
	}
	return notme;
}

/**
 * \param to code page
 * \param from code page
 * \param msg message
 */
std::string cautorespmod::convert( const string & to, const string & from, 
		const string & msg) {
	if( msg.empty() ) return "";
				
	iconv_t ic = iconv_open(to.c_str(), from.c_str());
	if( (iconv_t) -1 == ic ) 
			throw runtime_error((string)"cautoresp::convert("
				+to+","+from+"): iconv_open: "+strerror(errno));
	
	size_t in_len, buf_len = msg.length()*2, out_len;
	ICONV_IN_TYPE *in;
	char *buf = new char[buf_len], *out;
	for( ;; ) {
			in_len = msg.length();
			in = (ICONV_IN_TYPE *)msg.data();
			out = buf;
			out_len = buf_len;

			if( (size_t)-1 == iconv(ic, &in, &in_len, &out, &out_len) ) {
					if( errno == E2BIG ) {
							buf_len *= 2;
							delete [] buf;
							buf = new char[ buf_len ];
							continue;
					} else {
							iconv_close(ic);
							throw runtime_error((string)"cautoresp::convert("
								+to+","+from+"): iconv: "+strerror(errno));
					}
			}
			string ret(buf, out-buf);
			delete [] buf;
			iconv_close(ic);
			return ret;
	}
}
