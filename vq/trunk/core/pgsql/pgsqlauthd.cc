/*
Copyright (c) 2002,2003,2004 Pawel Niewiadomski
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
#include "vq_conf.h"
#include "sig.h"
#include "fd.h"
#include "fdstr.h"
#include "util.h"
#include "lock.h"
#include "pgsqlcommon.h"
#include "sys.h"
#include "cvq.h"
#include "cdaemonauth.h"
#include "cdaemonchild.h"
#include "cdaemonmaster.h"
#include "lower.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ctime>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>


using namespace std;
using namespace vq;
using namespace pqxx;

/**
 * 
 */
class cpgsqlauthd : public cdaemonchild {
		public:
				cpgsqlauthd();
				virtual ~cpgsqlauthd();
				
				void setup();
				int child( int );

				static cdaemonchild * alloc() { return new cpgsqlauthd(); }

		protected:
				/// socket
				int cso;

				/// pgsql
				Connection *pg;

				/// child
				string dom, user, pass, ip, ext;
				cauth::quota_value qb, qf;
				
				cvq::udot_type type;

				void cmd_dom_add();
				void cmd_dom_rm();
				void cmd_user_add();
				void cmd_user_pass();
				void cmd_user_rm();
				void cmd_user_auth();
				void cmd_user_ex();
				void cmd_dom_ip_add();
				void cmd_dom_ip_rm();
				void cmd_dom_ip_rm_all();
				void cmd_dom_ip_ls();
				void cmd_dom_ip_ls_dom();
				void cmd_udot_add() throw (out_of_range);
				void cmd_udot_ls();
				void cmd_udot_ls_type();
				void cmd_udot_rm();
				void cmd_udot_rm_type();
				void cmd_udot_get();
				void cmd_udot_rep();
				void cmd_udot_has();
				void cmd_udot_type_cnt();
				void cmd_qt_def_set();
				void cmd_qt_def_get();
				void cmd_qt_global_def_set();
				void cmd_qt_global_def_get();
				void cmd_qt_get();
				void cmd_qt_set();

				void qt_def_get(const std::string &);
				void qt_def_set(const std::string &);

				void (cpgsqlauthd::* cpgsqlauthd::cmd_proc( char cmd )) ();
};

/**
 *
 */
cpgsqlauthd::cpgsqlauthd() : pg(0) {
}

/**
 *
 */
cpgsqlauthd::~cpgsqlauthd() {
	delete pg;
}

/**
 * \exception runtime_error if it can't create socket
 */
void cpgsqlauthd::setup() {
	if( !(pg = new Connection(ac_pgsql.val_str().c_str())) ) 
			throw runtime_error((string)"setup: no memory"); 
}

/**
*/
void cpgsqlauthd::cmd_dom_add() {
	if( fdrdstr(cso, dom) != -1 ) {
			dom = lower(dom);
			Result res(NonTransaction(*pg).Exec(
					"SELECT DOM_ADD("+Quote(str2tb(dom), false)+","
					+Quote(dom, false)+","
					+Quote(pg->UserName(), false)+")"));
			
			if(res.empty() || res[0][0].is_null() 
				|| strcmp(res[0][0].c_str(), "t") ) {
					if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
					return;
			}

			if( fdwrite(cso, &"K", 1) != 1 ) throw wr_error();
	} else 
			throw rd_error();
}

/**
*/
void cpgsqlauthd::cmd_dom_rm() {
	if( fdrdstr(cso, dom) != -1 ) {
			dom = str2tb(lower(dom));
			Result res(NonTransaction(*pg).Exec(
					"SELECT DOM_RM("+Quote(dom, false)+")"));

			if(res.empty() || res[0][0].is_null() 
				|| strcmp(res[0][0].c_str(), "t") ) {
					if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
					return;
			}

			if( fdwrite(cso, &"K", 1) != 1 ) throw wr_error();
	} else 
			throw rd_error();
}

/**
*/
void cpgsqlauthd::cmd_user_add() {
	uint8_t flags;
	if( fdrdstr(cso, user) != -1
		&& fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, pass) != -1 
		&& fdread(cso, &flags, sizeof(flags)) == sizeof(flags) ) {
			dom = lower(str2tb(dom));
			user = lower(user);
			ostringstream sflags;
			sflags<<(int)flags;

			Result res(NonTransaction(*pg).Exec(
					"SELECT USER_ADD("+Quote(user, false)+","
					+Quote(dom, false)+","
					+Quote(pass, false)+","
					+Quote(sflags.str(), false)+")"));

			if(res.empty() || res[0][0].is_null() ) {
					if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
					return;
			}
			const char *val = res[0][0].c_str();
			if( *(val+1) == '\0' ) {
					if( *val == '1' ) {
							if( fdwrite(cso, &"D", 1) != 1 ) throw wr_error();
							return;
					} else if( *val == '0' ) {
							if( fdwrite(cso, &"K", 1) != 1 ) throw wr_error();
							return;
					}
			}
			if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
	} else 
			throw rd_error();
}

/**
 * 
 */
void cpgsqlauthd::cmd_user_pass() {
	if( fdrdstr(cso, user) != -1
		&& fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, pass) != -1 ) {
			dom = lower(str2tb(dom));
			user = lower(user);

			Result res(NonTransaction(*pg).Exec(
					"SELECT USER_PASS("+Quote(user, false)+","
					+Quote(dom, false)+","+Quote(pass, false)+")"));

			if(res.empty() || res[0][0].is_null() 
				|| strcmp(res[0][0].c_str(), "t") ) {
					if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
					return;
			}

			if( fdwrite(cso, &"K", 1) != 1 ) throw wr_error();
	} else 
			throw wr_error();
}

/**
*/
void cpgsqlauthd::cmd_user_rm() {
	if( fdrdstr(cso, user) != -1
		&& fdrdstr(cso, dom) != -1 ) {
			dom = lower(str2tb(dom));
			user = lower(user);
			Result res(NonTransaction(*pg).Exec(
					"SELECT USER_RM("+Quote(user, false)+","
					+Quote(dom, false)+")"));

			if(res.empty() || res[0][0].is_null() 
				|| strcmp(res[0][0].c_str(), "t") ) {
					if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
					return;
			}

			if( fdwrite(cso, &"K", 1) != 1 ) throw wr_error();
	} else 
			throw rd_error();
}

/**
*/
void cpgsqlauthd::cmd_user_auth() {
	if( fdrdstr(cso, user) != -1
	   && fdrdstr(cso, dom) != -1 ) {
			dom = lower(dom);
			user = lower(user);
			alias2dom(*pg, dom);
			string tb(str2tb(dom));

			Result res(NonTransaction(*pg).Exec(
				"SELECT pass,flags FROM \""+tb
				+"\" WHERE login="+Quote(user, false)));

			if( res.size() == 1 ) {
					if( res[0][0].is_null() || res[0][1].is_null() ) {
							if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
							return;
					}

					const char *pass = res[0][0].c_str();
					if(!pass) pass = "";
					const char *flags = res[0][1].c_str();
					if(!flags) flags= "0";
					istringstream is;
					is.str(flags);
					int iflags;
					is>>iflags;
					if( ! is ) {
							if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
							return;
					}
					if( fdwrite(cso,&"F",1) != 1
						|| fdwrstr(cso, user) == -1
						|| fdwrstr(cso, dom) == -1
						|| fdwrstr(cso, pass) == -1
						|| fdwrite(cso, &iflags, sizeof(iflags)) == -1 ) 
							throw wr_error();
			} else if(fdwrite(cso,&"Z",1)!=1) throw wr_error();
	} else throw rd_error();
}

/**
 */
void cpgsqlauthd::cmd_user_ex() {
	if( -1 != fdrdstr(cso, dom)
		&& -1 != fdrdstr(cso, user) ) {
			uint8_t ret=0xff;
			dom = lower(dom);
			user = lower(user);
			alias2dom(*pg, dom);
			dom = str2tb(dom);

			Result res(NonTransaction(*pg).Exec(
				"SELECT user_exists("+Quote(dom, false)+","
				+Quote(user, false)+')'));

			if( res.size() == 1 ) {
					istringstream is;
					unsigned ret1;
					const char *tmp = res[0][0].c_str();
					if(!tmp) tmp = "";
					is.str(tmp);
					is>>ret1;
					if(is) ret = ret1 & 0xff;
			}
			if( 1 != fdwrite(cso, &"F", 1) ) {
					switch(ret) {
					case 1:
							ret = cvq::err_user_nf;
							break;
					case 0:
							ret = cvq::err_no;
							break;
					case 2:
							ret = cvq::err_dom_inv;
							break;
					default:
							ret = cvq::err_temp;
					}
				
					if( sizeof(ret) == fdwrite(cso, &ret, sizeof(ret)) )
							return;
			}
			throw wr_error();
	} else throw rd_error();
}

/**
 *
 */
void cpgsqlauthd::cmd_dom_ip_add() {
	if( fdrdstr(cso,dom) != -1
		&& fdrdstr(cso,ip) != -1 ) {
			Result res(NonTransaction(*pg).Exec(
				"INSERT INTO ip2domain (DOMAIN,IP) VALUES("
				+Quote(lower(dom), false)+","
				+Quote(lower(ip), false)+')'));

			if(fdwrite(cso,&"K",1) != 1) throw wr_error();
	} else throw rd_error();
}

/**
*/
void cpgsqlauthd::cmd_dom_ip_rm() {
	if( fdrdstr(cso,dom) != -1
		&& fdrdstr(cso,ip) != -1 ) {
			Result res(NonTransaction(*pg).Exec(
				"DELETE FROM ip2domain WHERE DOMAIN="
				+Quote(lower(dom), false)
				+" AND IP="+Quote(lower(ip), false)));

			if(fdwrite(cso,&"K",1) != 1) throw wr_error();
	} else throw rd_error();
}
/**
*/
void cpgsqlauthd::cmd_dom_ip_rm_all() {
	if( fdrdstr(cso,dom) != -1 ) {
			Result res(NonTransaction(*pg).Exec(
				"DELETE FROM ip2domain WHERE DOMAIN="
				+Quote(lower(dom), false)));

			if(fdwrite(cso,&"K",1) != 1) throw wr_error();
	} else throw rd_error();
}
/**
 * 
 */
void cpgsqlauthd::cmd_dom_ip_ls() {
	if( fdrdstr(cso,dom) == -1 )
			throw rd_error();

	dom = lower(dom);
	
	Result res(NonTransaction(*pg).Exec(
		"SELECT IP FROM ip2domain WHERE DOMAIN="
		+Quote(dom, false)+" ORDER BY IP"));

	vector<string>::size_type i, cnt = res.size();
	if( fdwrite(cso,&"F",1) == -1
		|| fdwrite(cso,&cnt,sizeof(cnt)) == -1 )
			throw wr_error();

	string::size_type ipl;
	const char *ip;
	for(i=0; i<cnt; ++i) {
			ip = res[i][0].c_str();
			ipl = ! ip ? 0 : strlen(ip);
			if( fdwrite(cso,&ipl,sizeof(ipl)) == -1
				|| (ip && fdwrite(cso, ip, ipl) == -1) )
					throw wr_error();
	}
}
/**
*/
void cpgsqlauthd::cmd_dom_ip_ls_dom() {
	Result res(NonTransaction(*pg).Exec(
		"SELECT DISTINCT DOMAIN FROM ip2domain ORDER BY DOMAIN"));

	vector<string>::size_type i, cnt = res.size();
	if(fdwrite(cso,&"F",1) == -1
	   || fdwrite(cso,&cnt,sizeof(cnt)) == -1 )
			throw wr_error();

	string::size_type doml;
	const char *dom;
	for(i=0; i<cnt; ++i) {
			dom = res[i][0].c_str();
			doml= !dom ? 0 : strlen(dom);
			if( fdwrite(cso,&doml,sizeof(doml)) == -1
				|| (dom && fdwrite(cso, dom, doml) == -1) )
					throw wr_error();
	}
}

/**
 *
 */
void cpgsqlauthd::cmd_udot_add() throw (out_of_range) {
	string val;
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, user) == -1
	   || fdrdstr(cso, ext) == -1
	   || fdread(cso, &type, sizeof(type)) == -1 
	   || fdrdstr(cso, val) == -1 )
			throw rd_error();
	
	dom = lower(dom);
	user = lower(user);
	ext = lower(ext);
	
	dom = str2tb(dom);
	Result res(NonTransaction(*pg).Exec(
		"SELECT UDOT_ADD("+Quote(dom, false)+","+Quote(user, false)
		+","+Quote(ext, false)+",'\\"+(char)type+"',"
		+Quote(val, false)+")"));

	if(res.size() && !res[0][0].is_null()) {
			string id(res[0][0].c_str());
			if(fdwrite(cso,&"F",1) == -1 || fdwrstr(cso, id) == -1 )
					throw wr_error();
			return;
	}
	if(fdwrite(cso,&"E",1)!=1) throw wr_error();
}
/**
 *
 */
void cpgsqlauthd::cmd_udot_ls() {
	string ext;
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, user) == -1
	   || fdrdstr(cso, ext) == -1 )
			throw rd_error();
	
	dom = lower(str2tb(dom));
	user = lower(user);
	ext = lower(ext);
	Result res(NonTransaction(*pg).Exec(
		"SELECT ID,TYPE,VALUE FROM \""+dom+"_dot\" WHERE EXT="
		+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
		+" ORDER BY ID"));

	vector<string>::size_type i, cnt = res.size();
	if(fdwrite(cso,&"F",1) == -1
	   || fdwrite(cso,&cnt,sizeof(cnt)) == -1 )
			throw wr_error();

	const char *tmp;
	cvq::udot_info ui;
	for(i=0; i<cnt; ++i) {
			tmp = res[i][0].c_str();
			ui.id = !tmp ? "" : tmp;
			tmp = res[i][1].c_str();
			(char)ui.type = !tmp ? 0 : *tmp;
			tmp = res[i][2].c_str();
			ui.val = !tmp ? "" : tmp;

			if( fdwrstr(cso,ui.id) == -1 
				|| fdwrite(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
				|| fdwrstr(cso, ui.val) == -1 )
					throw wr_error();
	}
}
/**
 *
 */
void cpgsqlauthd::cmd_udot_ls_type() {
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, user) == -1
	   || fdrdstr(cso, ext) == -1
	   || fdread(cso, &type, sizeof(type)) == -1 )
			throw rd_error();

	dom = lower(str2tb(dom));
	user = lower(user);
	ext = lower(ext);
	Result res(NonTransaction(*pg).Exec(
		"SELECT ID,TYPE,VALUE FROM \""+dom+"_dot\" WHERE EXT="
		+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
		+" AND TYPE='\\"+(char)type+"' ORDER BY ID"));

	vector<string>::size_type i, cnt = res.size();
	if(fdwrite(cso,&"F",1) == -1
	   || fdwrite(cso,&cnt,sizeof(cnt)) == -1 )
			throw wr_error();

	const char *tmp;
	cvq::udot_info ui;
	for(i=0; i<cnt; ++i) {
			tmp = res[i][0].c_str();
			ui.id = !tmp ? "" : tmp;
			tmp = res[i][1].c_str();
			(char)ui.type = !tmp ? 0 : *tmp;
			tmp = res[i][2].c_str();
			ui.val = !tmp ? "" : tmp;
			if( fdwrstr(cso,ui.id) == -1 
				|| fdwrite(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
				|| fdwrstr(cso, ui.val) == -1 )
					throw wr_error();
	}
}
/**
 *
 */
void cpgsqlauthd::cmd_udot_rm() {
	string id;
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, id) == -1 )
			throw rd_error();

	dom = lower(str2tb(dom));
	Result res(NonTransaction(*pg).Exec(
		"DELETE FROM \""+dom+"_dot\" WHERE ID="+Quote(id, false)));

	if(fdwrite(cso, &"K", 1)!=1) throw wr_error();
}
/**
 *
 */
void cpgsqlauthd::cmd_udot_rm_type() {
	if( fdrdstr(cso, dom) == -1 
		|| fdrdstr(cso, user) == -1 
		|| fdrdstr(cso, ext) == -1
		|| fdread(cso, &type, sizeof(type)) != sizeof(type) )
			throw rd_error();

	dom = lower(str2tb(dom));
	user = lower(user);
	ext = lower(ext);
	Result res(NonTransaction(*pg).Exec(
		"DELETE FROM \""+dom+"_dot\" WHERE EXT="
		+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
		+" AND TYPE='\\"+(char)type+"'"));

	if(fdwrite(cso, &"K",1) != 1) throw wr_error();
}
/**
 *
 */
void cpgsqlauthd::cmd_udot_get() {
	string id;
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, id) == -1 )
			throw rd_error();
	
	dom = lower(str2tb(dom));
	Result res(NonTransaction(*pg).Exec(
		"SELECT TYPE,VALUE FROM \""+dom+"_dot\" WHERE ID="+Quote(id, false)));

	if(res.empty()) {
			if(fdwrite(cso, &"Z", 1)!=1) throw wr_error();
			return;
	}
	
	string val;
	const char *ptr;
	ptr = res[0][0].c_str();
	(char)type = ! ptr ? '\0' : *ptr;
	ptr = res[0][1].c_str();
	val = ! ptr ? "" : ptr;
	
	if( fdwrite(cso, &"F", 1) != 1 
	   || fdwrite(cso, &type, sizeof(type)) != sizeof(type)
	   || fdwrstr(cso, val) == -1 )
			throw wr_error();
}

/**
 *
 */
void cpgsqlauthd::cmd_udot_rep() {
	cvq::udot_info ui;

	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, ui.id) == -1
	   || fdread(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
	   || fdrdstr(cso, ui.val) == -1 )
			throw rd_error();

	dom = lower(str2tb(dom));
	Result res(NonTransaction(*pg).Exec(
		"UPDATE \""+dom+"_dot\" SET TYPE='\\"+(char)ui.type
		+"',VALUE="+Quote(ui.val, false)+" WHERE ID="
		+Quote(ui.id, false)));

	if( ! res.AffectedRows() ) {
			if(fdwrite(cso, &"Z", 1) != 1) throw wr_error();
	} else 
			if(fdwrite(cso, &"K", 1) != 1) throw wr_error();
}
/**
 *
 */
void cpgsqlauthd::cmd_udot_has() {
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, user) == -1
	   || fdrdstr(cso, ext) == -1
	   || fdread(cso, &type, sizeof(type)) == -1 )
			throw rd_error();

	dom = lower(str2tb(dom));
	user = lower(user);
	ext = lower(ext);
	Result res(NonTransaction(*pg).Exec(
		"SELECT 1 FROM \""+dom+"_dot\" WHERE ext="
		+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
		+" AND TYPE='\\"+(char)type+"' LIMIT 1"));

	if( ! res.empty() ) {
			if(fdwrite(cso, &"T", 1) != 1) throw wr_error();
	} else 
			if(fdwrite(cso, &"F",1) != 1) throw wr_error();
}
/**
 *
 */
void cpgsqlauthd::cmd_udot_type_cnt() {
	if( fdrdstr(cso, dom) == -1 
		|| fdrdstr(cso, user) == -1 
		|| fdrdstr(cso, ext) == -1
		|| fdread(cso, &type, sizeof(type)) != sizeof(type) )
			throw rd_error();
	
	dom = lower(str2tb(dom));
	user = lower(user);
	ext = lower(ext);
	Result res(NonTransaction(*pg).Exec(
		"SELECT COUNT(*) FROM \""+dom+"_dot\" WHERE ext="
		+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
		+" AND TYPE='\\"+(char)type+"'"));

	if( res.empty() || res[0][0].is_null() ) {
			if( fdwrite(cso, &"E", 1)) throw wr_error();
	}

	const char *ptr = res[0][0].c_str();

	cdaemonauth::size_type cnt;
	istringstream is(ptr);
	is>>cnt;
	if( ! is ) cnt = 0;
	
	if(fdwrite(cso, &"F",1) != 1
	   || fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt))
			throw wr_error();
}

/**
 *
 */
void cpgsqlauthd::qt_def_set( const string & d ) {
	dom = lower(d);
	if( -1 != fdread(cso, &qb, sizeof qb)
		&& -1 != fdread(cso, &qf, sizeof qf) ) {
			dom = lower(dom);

			ostringstream qb_str, qf_str;
			qb_str<<qb;
			qf_str<<qf;
			
			Result res(NonTransaction(*pg).Exec(
				"SELECT qt_def_set("
				+Quote(dom, false)+","
				+Quote(qb_str.str(), false)+","
				+Quote(qf_str.str(), false)+')'));

			if( fdwrite(cso, &"K", 1) != 1 ) 
					throw wr_error();
	} else throw rd_error();
}

/**
 *
 */
void cpgsqlauthd::cmd_qt_global_def_set() {
	qt_def_set("-");
}

/**
 * Gets default quota for new users and sends it via cso
 * \param dom domain
 */
void cpgsqlauthd::qt_def_get( const string & d ) {
	qb = qf = 0;
	dom = lower(d);
	Result res(NonTransaction(*pg).Exec(
		"SELECT qt_user_bytes_def,qt_user_files_def FROM domains"
		" WHERE name="+Quote(dom, false)+" LIMIT 1"));

	if( res.size() <= 0 ) {
			if( fdwrite(cso, &"Z", 1) != 1 )
					throw wr_error();
			return;
	}
	
	if( ! res[0][0].is_null() )
			res[0][0].to<cauth::quota_value>(qb);
	if( ! res[0][1].is_null() )
			res[0][1].to<cauth::quota_value>(qf);

	if( fdwrite(cso, &"F", 1) != 1
		|| fdwrite(cso, &qb, sizeof qb) != sizeof qb
		|| fdwrite(cso, &qf, sizeof qf) != sizeof qf) 
			throw wr_error();
}

/**
 * Gets limits for specified user
 */
void cpgsqlauthd::cmd_qt_get() {
	if( -1 == fdrdstr(cso, dom)
		|| -1 == fdrdstr(cso, user))
			throw rd_error();

	qb = qf = 0;
	dom = lower(str2tb(dom));
	user = lower(user);

	Result res(NonTransaction(*pg).Exec(
		"SELECT qt_bytes,qt_files FROM \""+dom
		+"\" WHERE login="+Quote(user, false)));

	if( res.size() <= 0 ) {
			if( fdwrite(cso, &"Z", 1) != 1 )
					throw wr_error();
			return;
	}
	
	if( ! res[0][0].is_null() )
			res[0][0].to<cauth::quota_value>(qb);
	if( ! res[0][1].is_null() )
			res[0][1].to<cauth::quota_value>(qf);

	if( fdwrite(cso, &"F", 1) != 1
		|| fdwrite(cso, &qb, sizeof qb) != sizeof qb
		|| fdwrite(cso, &qf, sizeof qf) != sizeof qf) 
			throw wr_error();
}

/**
 * Sets limits for specified user
 */
void cpgsqlauthd::cmd_qt_set() {
	if( -1 == fdrdstr(cso, dom)
		|| -1 == fdrdstr(cso, user)
		|| -1 == fdread(cso, &qb, sizeof qb)
		|| -1 == fdread(cso, &qf, sizeof qf) )
			throw rd_error();

	dom = str2tb(lower(dom));
	user = lower(user);

	Result res(NonTransaction(*pg).Exec(
		"UPDATE \""+dom+"\" SET qt_bytes="+ToString<cauth::quota_value>(qb)
		+",qt_files="+ToString<cauth::quota_value>(qf)
		+" WHERE login="+Quote(user, false)));

	if( fdwrite(cso, &"K", 1) != 1 ) 
			throw wr_error();
}

/**
 */
void cpgsqlauthd::cmd_qt_global_def_get() {
	qt_def_get("-");
}

/**
 */
void cpgsqlauthd::cmd_qt_def_get() {
	if( -1 != fdrdstr(cso, dom) ) {
			qt_def_get(dom);
	} else throw rd_error();
}

/**
 */
void cpgsqlauthd::cmd_qt_def_set() {
	if( -1 != fdrdstr(cso, dom) ) {
			qt_def_set(dom);
	} else throw rd_error();
}

/**
*/
void (cpgsqlauthd::* cpgsqlauthd::cmd_proc( char cmd )) () {
	switch(cmd) {
	case cdaemonauth::cmd_dom_add: 
		return &cpgsqlauthd::cmd_dom_add;
	case cdaemonauth::cmd_dom_rm:
		return &cpgsqlauthd::cmd_dom_rm;
	case cdaemonauth::cmd_user_add:
		return &cpgsqlauthd::cmd_user_add;
	case cdaemonauth::cmd_user_rm:
		return &cpgsqlauthd::cmd_user_rm;
	case cdaemonauth::cmd_user_pass:
		return &cpgsqlauthd::cmd_user_pass;
	case cdaemonauth::cmd_user_auth:
		return &cpgsqlauthd::cmd_user_auth;
	case cdaemonauth::cmd_dom_ip_add:
		return &cpgsqlauthd::cmd_dom_ip_add;
	case cdaemonauth::cmd_dom_ip_rm:
		return &cpgsqlauthd::cmd_dom_ip_rm;
	case cdaemonauth::cmd_dom_ip_rm_all:
		return &cpgsqlauthd::cmd_dom_ip_rm_all;
	case cdaemonauth::cmd_dom_ip_ls:
		return &cpgsqlauthd::cmd_dom_ip_ls;
	case cdaemonauth::cmd_dom_ip_ls_dom:
		return &cpgsqlauthd::cmd_dom_ip_ls_dom;
	case cdaemonauth::cmd_udot_add:
		return &cpgsqlauthd::cmd_udot_add;
	case cdaemonauth::cmd_udot_rm:
		return &cpgsqlauthd::cmd_udot_rm;
	case cdaemonauth::cmd_udot_rm_type:
		return &cpgsqlauthd::cmd_udot_rm_type;
	case cdaemonauth::cmd_udot_ls:
		return &cpgsqlauthd::cmd_udot_ls;
	case cdaemonauth::cmd_udot_ls_type:
		return &cpgsqlauthd::cmd_udot_ls_type;
	case cdaemonauth::cmd_udot_get:
		return &cpgsqlauthd::cmd_udot_get;
	case cdaemonauth::cmd_udot_rep:
		return &cpgsqlauthd::cmd_udot_rep;
	case cdaemonauth::cmd_udot_has:
		return &cpgsqlauthd::cmd_udot_has;
	case cdaemonauth::cmd_udot_type_cnt:
		return &cpgsqlauthd::cmd_udot_type_cnt;
	case cdaemonauth::cmd_user_ex:
		return &cpgsqlauthd::cmd_user_ex;
	case cdaemonauth::cmd_qt_def_set:
		return &cpgsqlauthd::cmd_qt_def_set;
	case cdaemonauth::cmd_qt_def_get:
		return &cpgsqlauthd::cmd_qt_def_get;
	case cdaemonauth::cmd_qt_global_def_get:
		return &cpgsqlauthd::cmd_qt_global_def_get;
	case cdaemonauth::cmd_qt_global_def_set:
		return &cpgsqlauthd::cmd_qt_global_def_set;
	case cdaemonauth::cmd_qt_get:
		return &cpgsqlauthd::cmd_qt_get;
	case cdaemonauth::cmd_qt_set:
		return &cpgsqlauthd::cmd_qt_set;
	default:
		return NULL;
	}
}
/**
 * \return 0 - ok, 1 - eof, 2 - error
 */
int cpgsqlauthd::child(int fd) {
	try {
			char cmd;
			void (cpgsqlauthd::* run)();

			cso = fd;
	
			switch( fdread(cso, &cmd, 1) ) {
			case 0: return 1;
			case -1: 
					throw rd_error();
			}

			// process cmd
			if( ! (run = cmd_proc(cmd)) ) {
					ostringstream os;
					os<<cmd;
					throw bad_command(os.str());
			}

			((*this).*run)();
			return 0;
	} catch( const rd_error & e ) {
			cerr<<"child: exception: "<<e.what()<<endl;
	} catch( const wr_error & e ) {
			cerr<<"child: exception: "<<e.what()<<endl;
	} catch( const exception & e ) {
			cerr<<"child: exception: "<<e.what()<<endl;
			if( fdwrite(cso, &"E", 1) != 1 ) cerr<<"child: write error: "<<strerror(errno)<<endl;
	}
	
	return 2;
}

/**
 * Plugin structure.
 */
cdaemonmaster::daemon_info pgsqlauthd_info = { & cpgsqlauthd::alloc };

