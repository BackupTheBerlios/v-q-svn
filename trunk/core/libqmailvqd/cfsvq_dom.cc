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
#include "pfstream.h"
#include "conf_home.h"
#include "vq_conf.h"
#include "cfsvq.h"
#include "lower.h"
#include "lock.h"
#include "dirs.h"
#include "split.h"
#include "util.h"
#include "auto/d_namlen.h"
#include "sys.h"
#include "qmail_progs.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <cerrno>
#include <inttypes.h>
#include <algorithm>
#include <dirent.h>
#include <signal.h>
#include <iomanip>

using namespace std;
using namespace posix;
using namespace vq;

/**
 * Remove aliass.
 * \param a alias name
 * \return err_no on success
 */
uint8_t cfsvq::dom_alias_rm( const string & a )
{
	char ret;
	string alias(lower(a));
	ret=assign_ex(alias);
	if( ret ) return(ret);
	return virt_rm(alias);
}

/**
 * Remove domain. It removes domain from qmail control file, authorization
 * then from filesystem. It ignores errors (so it may not remove something).
 * \return last error code returned by any called function. So only err_no means that everything was removed.
 */
uint8_t cfsvq::dom_rm(const string &d)
{
	assert_auth();
	string dom(lower(d));
	uint8_t ret;
	
	ret=rcpt_rm(dom);
	ret=morercpt_rm(dom);
	ret=virt_rm(dom);
	ret=assign_rm(dom);

	if(auth->dom_rm(dom)) ret = lr(err_auth, auth->err_report());

	string restart(conf_home+"/bin/qmail_run");
	char prog[] = { qp_send_restart, '\0' };
	char *const args[] = {
			const_cast<char *>(restart.c_str()),
			prog,
			NULL
	};
	int rr = run(args);
	if( ! WIFEXITED(rr) || WEXITSTATUS(rr) ) ret = err_temp;

	string dir(conf_home+"/domains/"+split_dom(dom)+'/'+dom);
	if(!rmdirrec(dir)) {
			ret = lr(err_wr, dir);
	}

	return(ret);
}

/**
 * Adds domain to system
 * \param d valid domain name
 * \return err_no on success
 */
uint8_t cfsvq::dom_add(const string &d)
{
	assert_auth();
	string dom(lower(d));
	if(!dom_val(dom)) return lr(err_dom_inv, dom);

	if(auth->dom_add(dom)) { 
			return lr(err_auth, auth->err_report()); 
	}

	string dom_add_dir(conf_home+"/domains/"+split_dom(dom)+'/'+dom);
	if(!mkdirhier(dom_add_dir.c_str(), 
		ac_vq_mode.val_int(), ac_vq_uid.val_int(), ac_vq_gid.val_int())) {
			auth->dom_rm(dom);
			return lr(err_mkdir, dom);
	}

	/* dot file with a call to deliver */
	string dotfile(dom_add_dir+"/.qmail-default");
	if(!dumpstring( dotfile, (string)"|"+conf_home+"/bin/deliver\n")
	   || chown(dotfile.c_str(), ac_vq_uid.val_int(), ac_vq_gid.val_int())
	   || chmod(dotfile.c_str(), ac_vq_fmode.val_int()) ) {
			rmdirrec(dom_add_dir); 
			auth->dom_rm(dom);
			return lr(err_wr, dotfile);
	}

	if( virt_add(dom,dom) ) {
			rmdirrec(dom_add_dir);
			auth->dom_rm(dom);
			return lastret;
	}
		
	if(locals_rm(dom)) {
			rmdirrec(dom_add_dir); 
			auth->dom_rm(dom);
			virt_rm(dom);
			return lastret; 
	}

	switch(rcpt_add(dom)) {
	case err_no: break;
	case err_over:
			if(morercpt_add(dom) == err_no) break;
	default:
			lastret_blkd=true;
			rmdirrec(dom_add_dir); 
			auth->dom_rm(dom);
			virt_rm(dom);
			lastret_blkd=false;
			return lastret; 
	}
	if(assign_add(dom)) {
			lastret_blkd=true;
			rmdirrec(dom_add_dir); 
			auth->dom_rm(dom);
			virt_rm(dom);
			lastret_blkd=false;
			return lastret;
	}

	string restart(conf_home+"/bin/qmail_run");
	char prog[] = { qp_send_restart, '\0' };
	char *const args[] = {
			const_cast<char *>(restart.c_str()),
			prog,
			NULL
	};
	int rr = run(args);
	if( ! WIFEXITED(rr) || WEXITSTATUS(rr) ) {
			lastret_blkd=true;
			rmdirrec(dom_add_dir); 
			auth->dom_rm(dom);
			virt_rm(dom);
			lastret_blkd=false;
			return lastret;
	}

	return lr(err_no, "");
}

/**
 * Add alias for domain
 * \param d domain
 * \param a alias
 * \return err_no on success
 */
uint8_t cfsvq::dom_alias_add(const string &d, const string &a)
{
	string dom(lower(d)), ali(lower(a));
	if( virt_add(ali,dom) )
			return lastret;
	
	switch( rcpt_add(ali) ) {
	case err_no: break;
	case err_over:
			if(morercpt_add(dom) == err_no) break;
	default:
			lastret_blkd = true;
			virt_rm(ali);
			lastret_blkd = false;
			return lastret;
	}

	string restart(conf_home+"/bin/qmail_run");
	char prog[] = { qp_send_restart, '\0' };
	char *const args[] = {
			const_cast<char *>(restart.c_str()),
			prog,
			NULL
	};
	int rr = run(args);
	if( ! WIFEXITED(rr) || WEXITSTATUS(rr) ) {
			lastret_blkd = true;
			virt_rm(ali);
			lastret_blkd = false;
			return lr(err_exec, restart);
	}
	return lr(err_no, "");
}

/**
 * Adds IP address for domain
 * \param d domain
 * \param i IP
 * \return err_no on success
 */
uint8_t cfsvq::dom_ip_add(const string &d, const string &i)
{
	assert_auth();
	if(auth->dom_ip_add(lower(d),lower(i)))
			return lr(err_auth, auth->err_report());
	return lr(err_no, "");
}

/**
 * Remove IP address of domain
 * \param d domain's name
 * \param i IP
 * \return err_no on success
 */
uint8_t cfsvq::dom_ip_rm(const string &d, const string &i)
{
	assert_auth();
	if(auth->dom_ip_rm(lower(d),lower(i)))
			return lr(err_auth, auth->err_report());
	return lr(err_no, "");
}

/**
 * Remove all IP addresses asociated with domain
 * \param d domain
 * \return err_no on success
 */
uint8_t cfsvq::dom_ip_rm_all(const string &d)
{
	assert_auth();
	if(auth->dom_ip_rm_all(lower(d)))
			return lr(err_auth, auth->err_report());
	return lr(err_no, "");
}

/**
 * Lists IP addresses asociated with domain
 * \param d domain
 * \param ips set to list of IP addresses
 * \return err_no on success
 */
uint8_t cfsvq::dom_ip_ls(const string &d, vector<string> &ips)
{
	assert_auth();
	if(auth->dom_ip_ls(lower(d),ips))
			return lr(err_auth, auth->err_report());
	return lr(err_no, "");
}

/**
 * Lists all domains which have asociated IP addresses
 * \param doms list of domains
 * \return err_no on success
 */
uint8_t cfsvq::dom_ip_ls_dom(vector<string> &doms)
{
	assert_auth();
	if(auth->dom_ip_ls_dom(doms))
			return lr(err_auth, auth->err_report());
	return lr(err_no, "");
}

/**
 * Lock deliveries to specified domain
 * \return 0 on success
 */
uint8_t cfsvq::dom_del_lock( const string & dom ) {
	struct stat st;
	if(stat(dom.c_str(), &st)) return lr(err_stat, dom);
	if(! S_ISDIR(st.st_mode)) return lr(err_notdir, dom);
	if( chmod(dom.c_str(), (st.st_mode & 07777) | 01000) )
			return lr(err_chmod, dom );
	return lr(err_no, "");
}

/**
 * \brief unlock delivieries to specified domain
 * \return 0 on success
 */
uint8_t cfsvq::dom_del_unlock( const string & dom ) {
	struct stat st;
	if(stat(dom.c_str(), &st)) return lr(err_stat, dom);
	if(! S_ISDIR(st.st_mode)) return lr(err_notdir, dom);
	if(chmod(dom.c_str(), (st.st_mode & 07777) & ~01000))
			return lr(err_chmod, dom );
	return lr(err_no, "");
}

/**
 * Create path for specified domain (in vq directory)
 * \param dom domain
 * \return path (ie. /var/vq/domains/pl/org/test/test.org.pl)
 */
string cfsvq::dompath(const string &dom) {
	string ret;
	ret.reserve(conf_home.length()+10+2*dom.length());
	ret = conf_home;
	ret += "/domains/";
	ret += split_dom(dom);
	ret += dom;
	return ret;
}


