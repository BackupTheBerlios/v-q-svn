/*
Copyright (c) 2002 Pawel Niewiadomski
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
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>

#include "cdaemonauth.h"
#include "conf_home.h"
#include "fd.h"
#include "fdstr.h"
#include "lower.h"

using namespace std;
using namespace vq;

/**
 * Allocate cdaemonauth object
 */
cauth * cauth::alloc() {
	return new cdaemonauth();
}

/**
@exception runtime_error if it can't create socket
*/
cdaemonauth::cdaemonauth()
{
	int curdirfd = open(".",O_RDONLY);
	if(curdirfd==-1) 
			throw runtime_error((string)"cdaemonauth: open current dir: "+strerror(errno));
	if(chdir((conf_home+"/sockets").c_str()))
			throw runtime_error((string)"cdaemonauth: chdir: "
					+conf_home+"/sockets: "+strerror(errno));
	if( (cso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
			throw runtime_error((string)"cdaemonauth: socket: "+strerror(errno));
	cun.sun_family = AF_UNIX;
	strcpy(cun.sun_path, "daemonauth");
	if(connect(cso, (struct sockaddr*) &cun, SUN_LEN(&cun)))
			throw runtime_error((string)"cdaemonauth: connect: daemonauth: "+strerror(errno));
	if( fchdir(curdirfd) )
			throw runtime_error((string)"cdaemonauth: fchdir: "+conf_home
					+": "+strerror(errno));
}

/**
*/
cdaemonauth::~cdaemonauth()
{
	close(cso);
}
/**
*/
uint8_t cdaemonauth::dom_add( const string & dom ) {
	char resp=0;
	cmd = cmd_dom_add; 
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return lr(cvq::err_no);
	
	return ! resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}

/**
*/
uint8_t cdaemonauth::dom_rm( const string & dom )
{
	char resp=0;
	cmd = cmd_dom_rm;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return lr(cvq::err_no);
	
	return ! resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}
/**
 * \param u user's name
 * \param dom domain's name
 * \param p password
 * \param flags
 * \see cvq::user_add
 */
uint8_t cdaemonauth::user_add( const string & u, const string & dom, 
		const string & p, uint8_t flags )
{
	char resp=0;
	cmd = cmd_user_add;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(u)) != -1
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, p) != -1
		&& fdwrite(cso, &flags, sizeof(flags)) == sizeof(flags)
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return lr(cvq::err_no);
	
	if( ! resp ) {
			return lr(cvq::err_rd);
	} else if( resp == 'D' ) {
			return lr(cvq::err_user_inv, u);
	}
	return lr(cvq::err_temp);
}
/**
*/
uint8_t cdaemonauth::user_rm(const string & u, const string & dom ) {
	char resp=0;
	cmd = cmd_user_rm;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(u)) != -1
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return lr(cvq::err_no);
	
	return ! resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}
/**
*/
uint8_t cdaemonauth::user_pass( const string & u, const string & dom, 
		const string & p ) {
	char resp=0;
	cmd = cmd_user_pass;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(u)) != -1
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, p) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'K' ) return lr(cvq::err_no);
	
	return ! resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}

/**
*/
uint8_t cdaemonauth::user_auth(cvq::auth_info & ai) {
	char resp=0;
	cmd = cmd_user_auth;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(ai.user)) != -1
		&& fdwrstr(cso, lower(ai.dom)) != -1
		&& fdread(cso, &resp, sizeof(resp)) != -1
		&& resp=='F'
		&& fdrdstr(cso, ai.user) != -1
		&& fdrdstr(cso, ai.dom) != -1 
		&& fdrdstr(cso, ai.pass) != -1
		&& fdread(cso,&ai.flags,sizeof(ai.flags)) != -1 ) return lr(cvq::err_no);
	
	if( ! resp ) {
			return lr(cvq::err_rd);
	} else if(resp == 'E') {
			return lr(cvq::err_temp);
	}
	return lr(cvq::err_user_nf);
}
/**
*/
uint8_t cdaemonauth::dom_ip_add(const string &dom, const string &ip) {
	char resp=0;
	cmd = cmd_dom_ip_add;
	if( fdwrite(cso,&cmd,sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso,lower(dom)) != -1
		&& fdwrstr(cso,lower(ip)) != -1
		&& fdread(cso,&resp,sizeof(resp)) != -1
		&& resp == 'K' ) return lr(cvq::err_no);
	
	return !resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}

/**
*/
uint8_t cdaemonauth::dom_ip_rm(const string &dom,const string &ip) {
	char resp=0;
	cmd = cmd_dom_ip_rm;
	if( fdwrite(cso,&cmd,sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso,lower(dom)) != -1
		&& fdwrstr(cso,lower(ip)) != -1
		&& fdread(cso,&resp,sizeof(resp)) != -1 
		&& resp == 'K' ) return lr(cvq::err_no);
	
	return !resp ? lr(cvq::err_no) : lr(cvq::err_temp);
}

/**
*/
uint8_t cdaemonauth::dom_ip_rm_all(const string &dom) {
	char resp=0;
	cmd = cmd_dom_ip_rm_all;
	if( fdwrite(cso,&cmd,sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso,lower(dom)) != -1
		&& fdread(cso,&resp,sizeof(resp)) != -1 
		&& resp == 'K' ) return lr(cvq::err_no);
	
	return !resp ? lr(cvq::err_no) : lr(cvq::err_temp);
}
/**
*/
uint8_t cdaemonauth::dom_ip_ls(const string &dom, vector<string> &ips) {
	char resp=0;
	vector<string>::size_type cnt;
	cmd = cmd_dom_ip_ls;
	ips.clear();
	if( fdwrite(cso, &cmd,sizeof(cmd)) == sizeof(cmd) 
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdread(cso,&resp,sizeof(resp)) != -1
		&& resp == 'F'
		&& fdread(cso, &cnt, sizeof(cnt)) != -1 ) {
			ips.reserve(cnt);
			if(cnt--) {
					string ip;
					do {
							if( fdrdstr(cso, ip) == -1 )
									return lr(cvq::err_rd);
							ips.push_back(ip);
					} while(cnt--);
			}
			return lr(cvq::err_no);
	}
	
	return !resp ? lr(cvq::err_no) : lr(cvq::err_temp);
}
/**
*/
uint8_t cdaemonauth::dom_ip_ls_dom(vector<string> & doms) {
	char resp=0;
	vector<string>::size_type cnt;
	cmd = cmd_dom_ip_ls_dom;
	doms.clear();
	if( fdwrite(cso, &cmd,sizeof(cmd)) == sizeof(cmd) 
		&& fdread(cso,&resp,sizeof(resp)) != -1
		&& resp == 'F'
		&& fdread(cso, &cnt, sizeof(cnt)) != -1 ) {
			doms.reserve(cnt);
			if(cnt--) {
					string dom;
					do {
							if( fdrdstr(cso, dom) == -1 )
									return lr(cvq::err_rd);
							doms.push_back(dom);
					} while(cnt--);
			}
			return lr(cvq::err_no);
	}

	return !resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}
/**
 *
 */
uint8_t cdaemonauth::udot_add(const string &dom, const string &user,
		const string &ext, cvq::udot_info & ui ) {
	char resp=0;
	cmd = cmd_udot_add;
	if( fdwrite(cso,&cmd,sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, lower(user)) != -1
		&& fdwrstr(cso, lower(ext)) != -1
		&& fdwrite(cso, &ui.type, sizeof(ui.type)) != -1
		&& fdwrstr(cso, ui.val) != -1
		&& fdread(cso,&resp,sizeof(resp)) != -1 
		&& resp == 'F'
		&& fdrdstr(cso, ui.id) != -1) return lr(cvq::err_no);

	return !resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}
/**
 *
 */
uint8_t cdaemonauth::udot_rm(const string &dom, const string &id) {
	char resp=0;
	cmd = cmd_udot_rm;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, id) != -1
		&& fdread(cso, &resp, sizeof(resp)) != -1
		&& resp == 'K' ) return lr(cvq::err_no);

	return !resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}
/**
 *
 */
uint8_t cdaemonauth::udot_rm(const string &dom, const string &user, const string &ext,
		cvq::udot_type type ) {
	 resp = 0;
	 cmd = cmd_udot_rm_type;
	 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		 && fdwrstr(cso, lower(dom)) != -1
		 && fdwrstr(cso, lower(user)) != -1 
		 && fdwrstr(cso, lower(ext)) != -1 
		 && fdwrite(cso, &type, sizeof(type)) == sizeof(type)
		 && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		 && 'K' == resp ) return lr(cvq::err_no);

	 return !resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}
/**
 *
 */
uint8_t cdaemonauth::udot_ls(const string &dom, const string &user,
		const string &ext, vector<cvq::udot_info> & dots ) {
	resp = 0;
	cmd = cmd_udot_ls;
	vector<string>::size_type cnt;
	dots.clear();
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, lower(user)) != -1
		&& fdwrstr(cso, lower(ext)) != -1 
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'F' 
		&& fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
			dots.reserve(cnt);
			if(cnt--) {
					cvq::udot_info ui;
					do {
							if( fdrdstr(cso, ui.id) == -1
								|| fdread(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
								|| fdrdstr(cso, ui.val) == -1 )
									return lr(cvq::err_rd);
							dots.push_back(ui);
					} while(cnt--);
			}
			return lr(cvq::err_no);
	}

	return !resp ? lr(cvq::err_no) : lr(cvq::err_temp);
}
/**
 *
 */
uint8_t cdaemonauth::udot_ls(const string &dom, const string &user,
		const string &ext, cvq::udot_type type, vector<cvq::udot_info> & dots ) {
	resp = 0;
	cmd = cmd_udot_ls_type;
	vector<string>::size_type cnt;
	dots.clear();
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, lower(user)) != -1
		&& fdwrstr(cso, lower(ext)) != -1 
		&& fdwrite(cso, &type, sizeof(type)) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'F' 
		&& fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
			dots.reserve(cnt);
			if(cnt--) {
					cvq::udot_info ui;
					do {
							if( fdrdstr(cso, ui.id) == -1
								|| fdread(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
								|| fdrdstr(cso, ui.val) == -1 )
									return lr(cvq::err_rd);
							dots.push_back(ui);
					} while(cnt--);
			}
			return lr(cvq::err_no);
	}

	return !resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}
/**
 *
 */
uint8_t cdaemonauth::udot_rep(const string &dom, const cvq::udot_info & ui) {
	resp = 0;
	cmd = cmd_udot_rep;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
	   && fdwrstr(cso, lower(dom)) != -1
	   && fdwrstr(cso, ui.id) != -1
	   && fdwrite(cso, &ui.type, sizeof(ui.type)) == sizeof(ui.type)
	   && fdwrstr(cso, ui.val) != -1 
	   && fdread(cso, &resp, sizeof(resp)) == sizeof(resp) 
	   && resp=='K' ) {
			return lr(cvq::err_no);
	}
	
	if(!resp) {
			return lr(cvq::err_rd);
	} else if(resp == 'Z' ) {
			return lr(cvq::err_noent);
	} else {
			return lr(cvq::err_temp);
	}
}
/**
 *
 */
uint8_t cdaemonauth::udot_get(const string &dom,
		cvq::udot_info &ui) {
	resp = 0;
	cmd = cmd_udot_get;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, ui.id) != -1
		&& fdread(cso, &resp, sizeof(resp)) != -1
		&& resp == 'F'
		&& fdread(cso, &ui.type, sizeof(ui.type)) != -1
		&& fdrdstr(cso, ui.val) != -1) {
			return lr(cvq::err_no);
	}
	if(!resp) {
			return lr(cvq::err_rd);
	} else if(resp == 'Z') {
			return lr(cvq::err_noent);
	} else {
			return lr(cvq::err_temp);
	}
}

/**
 *
 */
uint8_t cdaemonauth::udot_has(const string &dom, const string &user, const string &ext,
		cvq::udot_type type ) {
	resp = 0;
	cmd = cmd_udot_has;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, lower(user)) != -1
		&& fdwrstr(cso, lower(ext)) != -1
		&& fdwrite(cso, &type, sizeof(type)) == sizeof(type)
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp) ) {
		switch(resp) {
		case 'F': return lr(cvq::err_noent);
		case 'T': return lr(cvq::err_no);
		}
	}
	return lr(cvq::err_rd);
}

/**
 *
 */
uint8_t cdaemonauth::udot_type_cnt(const string &dom, const string &user, 
		const string &ext, cvq::udot_type type, cvq::size_type & cnt ) {
	resp = 0;
	cmd = cmd_udot_type_cnt;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1 
		&& fdwrstr(cso, lower(user)) != -1 
		&& fdwrstr(cso, lower(ext)) != -1
		&& fdwrite(cso, &type, sizeof(type)) == sizeof(type)
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'F'
		&& fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
			return lr(cvq::err_no);
	}

	return ! resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}

/**
 *
 */
uint8_t cdaemonauth::user_ex(const string &dom, const string &user) {
	uint8_t ret;
	
	resp = 0;
	cmd = cmd_user_ex;
	if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
		&& fdwrstr(cso, lower(dom)) != -1
		&& fdwrstr(cso, lower(user)) != -1
		&& fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
		&& resp == 'F'
		&& fdread(cso, &ret, sizeof(ret)) == sizeof(ret) ) return lr(ret);

	return ! resp ? lr(cvq::err_rd) : lr(cvq::err_temp);
}
