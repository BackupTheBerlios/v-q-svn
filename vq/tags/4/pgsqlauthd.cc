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

#include "conf_home.h"
#include "vq_conf.h"
#include "sig.h"
#include "fd.h"
#include "fdstr.h"
#include "util.h"
#include "lock.h"
#include "pgsqlcommon.h"
#include "sys.h"
#include "daemon.h"
#include "cvq.h"
#include "cdaemonauth.h"

using namespace std;
using namespace vq;

// socket
int slock;
// pgsql
pqxx::Connection *pg = NULL;
// child
string dom, user, pass, ip, ext;
cvq::udot_type type;

/**
@exception runtime_error if it can't create socket
*/
void setup()
{
	umask(0);
	if(!sig_pipeign() || !sig_chld_nocldwait() ) 
			throw runtime_error((string)"setup: can't set signals: "+strerror(errno));
	if(chdir((conf_home+"/sockets").c_str()))
			throw runtime_error((string)"setup: chdir: "
					+conf_home+"/sockets: "+strerror(errno));
	slock = open("daemonauth.lock", O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
	if(slock==-1)
			throw runtime_error((string)"setup: open: daemonauth.lock: "+strerror(errno));
	if(lock_exnb(slock)) {
			if( errno == EWOULDBLOCK )
					throw runtime_error((string)"setup: daemonauth.lock is already locked");
			else throw runtime_error((string)"setup: daemonauth.lock: lock: "+strerror(errno));
	}
	if(unlink("daemonauth")==-1 && errno != ENOENT )
			throw runtime_error((string)"setup: unlink: daemonauth: "+strerror(errno));
	if( (sso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
			throw runtime_error((string)"setup: socket: "+strerror(errno));

	struct sockaddr_un sun;
	sun.sun_family = AF_UNIX;
	strcpy(sun.sun_path, "daemonauth");
	if(bind(sso, (struct sockaddr*) &sun, SUN_LEN(&sun)))
			throw runtime_error((string)"setup: bind: daemonauth: "+strerror(errno));
	if( !(pg = new pqxx::Connection(ac_pgsql.val_str().c_str())) ) 
			throw runtime_error((string)"setup: no memory"); 
	if( listen(sso, 5) == -1 )
			throw runtime_error((string)"setup: listen: "+strerror(errno));
}

/**
*/
void cmd_dom_add()
{
	if( fdrdstr(cso, dom) != -1 ) {
			str2tb(dom);
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
					"SELECT DOM_ADD("+pqxx::Quote(dom, false)+","
					+pqxx::Quote(pg->UserName(), false)+")"));
			
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
void cmd_dom_rm()
{
	if( fdrdstr(cso, dom) != -1 ) {
			str2tb(dom);
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
					"SELECT DOM_RM("+pqxx::Quote(dom, false)+")"));

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
void cmd_user_add()
{
	uint8_t flags;
	if( fdrdstr(cso, user) != -1
		&& fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, pass) != -1 
		&& fdread(cso, &flags, sizeof(flags)) == sizeof(flags) ) {
			str2tb(dom);
			ostringstream sflags;
			sflags<<(int)flags;

			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
					"SELECT USER_ADD("+pqxx::Quote(user, false)+","
					+pqxx::Quote(dom, false)+","
					+pqxx::Quote(pass, false)+","
					+pqxx::Quote(sflags.str(), false)+")"));

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
void cmd_user_pass()
{
	if( fdrdstr(cso, user) != -1
		&& fdrdstr(cso, dom) != -1
		&& fdrdstr(cso, pass) != -1 ) {
			str2tb(dom);

			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
					"SELECT USER_PASS("+pqxx::Quote(user, false)+","
					+pqxx::Quote(dom, false)+","+pqxx::Quote(pass, false)+")"));

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
void cmd_user_rm()
{
	if( fdrdstr(cso, user) != -1
		&& fdrdstr(cso, dom) != -1 ) {
			str2tb(dom);
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
					"SELECT USER_RM("+pqxx::Quote(user, false)+","
					+pqxx::Quote(dom, false)+")"));

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
void cmd_user_auth()
{
	if( fdrdstr(cso, user) != -1
	   && fdrdstr(cso, dom) != -1 ) {
			alias2dom(*pg, dom);
			string tb(dom);
			str2tb(tb);

			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT pass,flags FROM \""+tb
				+"\" WHERE login="+pqxx::Quote(user, false)));

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
void cmd_user_ex() {
	if( -1 != fdrdstr(cso, dom)
		&& -1 != fdrdstr(cso, user) ) {
			uint8_t ret=0xff;
			alias2dom(*pg, dom);
			str2tb(dom);

			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT user_exists("+pqxx::Quote(dom, false)+","
				+pqxx::Quote(user, false)+')'));

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
*/
void cmd_dom_ip_add()
{
	if( fdrdstr(cso,dom) != -1
		&& fdrdstr(cso,ip) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"INSERT INTO ip2domain (DOMAIN,IP) VALUES("
				+pqxx::Quote(dom, false)+","+pqxx::Quote(ip, false)+')'));

			if(fdwrite(cso,&"K",1) != 1) throw wr_error();
	} else throw rd_error();
}

/**
*/
void cmd_dom_ip_rm()
{
	if( fdrdstr(cso,dom) != -1
		&& fdrdstr(cso,ip) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"DELETE FROM ip2domain WHERE DOMAIN="
				+pqxx::Quote(dom, false)+" AND IP="+pqxx::Quote(ip, false)));

			if(fdwrite(cso,&"K",1) != 1) throw wr_error();
	} else throw rd_error();
}
/**
*/
void cmd_dom_ip_rm_all()
{
	if( fdrdstr(cso,dom) != -1 ) {
			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"DELETE FROM ip2domain WHERE DOMAIN="
				+pqxx::Quote(dom, false)));

			if(fdwrite(cso,&"K",1) != 1) throw wr_error();
	} else throw rd_error();
}
/**
 * 
 */
void cmd_dom_ip_ls()
{
	if( fdrdstr(cso,dom) == -1 )
			throw rd_error();
	
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT IP FROM ip2domain WHERE DOMAIN="
		+pqxx::Quote(dom, false)+" ORDER BY IP"));

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
void cmd_dom_ip_ls_dom()
{
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
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
void cmd_udot_add() throw (out_of_range) {
	string val;
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, user) == -1
	   || fdrdstr(cso, ext) == -1
	   || fdread(cso, &type, sizeof(type)) == -1 
	   || fdrdstr(cso, val) == -1 )
			throw rd_error();

	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT UDOT_ADD("+pqxx::Quote(dom, false)+","+pqxx::Quote(user, false)
		+","+pqxx::Quote(ext, false)+",'\\"+(char)type+"',"
		+pqxx::Quote(val, false)+")"));

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
void cmd_udot_ls() {
	string ext;
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, user) == -1
	   || fdrdstr(cso, ext) == -1 )
			throw rd_error();
	
	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT ID,TYPE,VALUE FROM \""+dom+"_dot\" WHERE EXT="
		+pqxx::Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
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
void cmd_udot_ls_type() {
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, user) == -1
	   || fdrdstr(cso, ext) == -1
	   || fdread(cso, &type, sizeof(type)) == -1 )
			throw rd_error();

	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT ID,TYPE,VALUE FROM \""+dom+"_dot\" WHERE EXT="
		+pqxx::Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
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
void cmd_udot_rm() {
	string id;
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, id) == -1 )
			throw rd_error();

	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"DELETE FROM \""+dom+"_dot\" WHERE ID="+pqxx::Quote(id, false)));

	if(fdwrite(cso, &"K", 1)!=1) throw wr_error();
}
/**
 *
 */
void cmd_udot_rm_type() {
	if( fdrdstr(cso, dom) == -1 
		|| fdrdstr(cso, user) == -1 
		|| fdrdstr(cso, ext) == -1
		|| fdread(cso, &type, sizeof(type)) != sizeof(type) )
			throw rd_error();

	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"DELETE FROM \""+dom+"_dot\" WHERE EXT="
		+pqxx::Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
		+" AND TYPE='\\"+(char)type+"'"));

	if(fdwrite(cso, &"K",1) != 1) throw wr_error();
}
/**
 *
 */
void cmd_udot_get() {
	string id;
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, id) == -1 )
			throw rd_error();
	
	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT TYPE,VALUE FROM \""+dom+"_dot\" WHERE ID="+pqxx::Quote(id, false)));

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
void cmd_udot_rep() {
	cvq::udot_info ui;

	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, ui.id) == -1
	   || fdread(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
	   || fdrdstr(cso, ui.val) == -1 )
			throw rd_error();

	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"UPDATE \""+dom+"_dot\" SET TYPE='\\"+(char)ui.type
		+"',VALUE="+pqxx::Quote(ui.val, false)+" WHERE ID="
		+pqxx::Quote(ui.id, false)));

	if( ! res.AffectedRows() ) {
			if(fdwrite(cso, &"Z", 1) != 1) throw wr_error();
	} else 
			if(fdwrite(cso, &"K", 1) != 1) throw wr_error();
}
/**
 *
 */
void cmd_udot_has() {
	if( fdrdstr(cso, dom) == -1
	   || fdrdstr(cso, user) == -1
	   || fdrdstr(cso, ext) == -1
	   || fdread(cso, &type, sizeof(type)) == -1 )
			throw rd_error();

	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT 1 FROM \""+dom+"_dot\" WHERE ext="
		+pqxx::Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
		+" AND TYPE='\\"+(char)type+"' LIMIT 1"));

	if( ! res.empty() ) {
			if(fdwrite(cso, &"T", 1) != 1) throw wr_error();
	} else 
			if(fdwrite(cso, &"F",1) != 1) throw wr_error();
}
/**
 *
 */
void cmd_udot_type_cnt() {
	if( fdrdstr(cso, dom) == -1 
		|| fdrdstr(cso, user) == -1 
		|| fdrdstr(cso, ext) == -1
		|| fdread(cso, &type, sizeof(type)) != sizeof(type) )
			throw rd_error();
	
	str2tb(dom);
	pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
		"SELECT COUNT(*) FROM \""+dom+"_dot\" WHERE ext="
		+pqxx::Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
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
*/
void (*cmd_proc( char cmd )) () {
	switch(cmd) {
	case cdaemonauth::cmd_dom_add: 
		return &cmd_dom_add;
	case cdaemonauth::cmd_dom_rm:
		return &cmd_dom_rm;
	case cdaemonauth::cmd_user_add:
		return &cmd_user_add;
	case cdaemonauth::cmd_user_rm:
		return &cmd_user_rm;
	case cdaemonauth::cmd_user_pass:
		return &cmd_user_pass;
	case cdaemonauth::cmd_user_auth:
		return &cmd_user_auth;
	case cdaemonauth::cmd_dom_ip_add:
		return &cmd_dom_ip_add;
	case cdaemonauth::cmd_dom_ip_rm:
		return &cmd_dom_ip_rm;
	case cdaemonauth::cmd_dom_ip_rm_all:
		return &cmd_dom_ip_rm_all;
	case cdaemonauth::cmd_dom_ip_ls:
		return &cmd_dom_ip_ls;
	case cdaemonauth::cmd_dom_ip_ls_dom:
		return &cmd_dom_ip_ls_dom;
	case cdaemonauth::cmd_udot_add:
		return &cmd_udot_add;
	case cdaemonauth::cmd_udot_rm:
		return &cmd_udot_rm;
	case cdaemonauth::cmd_udot_rm_type:
		return &cmd_udot_rm_type;
	case cdaemonauth::cmd_udot_ls:
		return &cmd_udot_ls;
	case cdaemonauth::cmd_udot_ls_type:
		return &cmd_udot_ls_type;
	case cdaemonauth::cmd_udot_get:
		return &cmd_udot_get;
	case cdaemonauth::cmd_udot_rep:
		return &cmd_udot_rep;
	case cdaemonauth::cmd_udot_has:
		return &cmd_udot_has;
	case cdaemonauth::cmd_udot_type_cnt:
		return &cmd_udot_type_cnt;
	case cdaemonauth::cmd_user_ex:
		return &cmd_user_ex;
	default:
		return NULL;
	}
}
/**
 * \return 0 - ok, 1 - eof, 2 - error
 */
char child(int fd)
{
	char cmd;
	void (*run)();

	cso = fd;
	
	switch( fdread(cso, &cmd, 1) ) {
	case 0: return 1;
	case -1: 
			cerr<<"child: read error: "<<strerror(errno)<<endl;
			return 2;
	}
	// process cmd
	if( ! (run = cmd_proc(cmd)) ) {
			cerr << "child: unknown command: " << (int) cmd << endl;
			return 2;
	}

	try {
			(*run)();
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

