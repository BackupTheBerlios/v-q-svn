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
#include "cqmailvq.hpp"
#include "vq_conf.hpp"
//#include "auto/d_namlen.h"
//#include "qmail_progs.h"

#include <pfstream.hpp>
#include <sys.hpp>
#include <text.hpp>

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

namespace POA_vq {

	using namespace std;
	using namespace posix;
	using namespace text;
	using namespace sys;
	
#if 0
	/**
	 * Remove aliass.
	 * \param a alias name
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error cqmailvq::dom_alias_rm( const char * a )
	{
		error ret;
		string alias(lower(a));
		ret=assign_ex(alias);
		if( ret != ::vq::ivq::err_no ) return(ret);
		return virt_rm(alias);
	}
	
	/**
	 * Remove domain. It removes domain from qmail control file, authorization
	 * then from filesystem. It ignores errors (so it may not remove something).
	 * \return last cqmailvq::error code returned by any called function. So only ::vq::ivq::err_no means that everything was removed.
	 */
	cqmailvq::error cqmailvq::dom_rm(const char *d)
	{
		assert_auth();
		string dom(lower(d));
		error ret;
		
		ret=rcpt_rm(dom);
		ret=morercpt_rm(dom);
		ret=virt_rm(dom);
		ret=assign_rm(dom);
	
		if(auth->dom_rm(dom.c_str())) ret = lr(::vq::ivq::err_auth, auth->err_report());
	
		string restart(conf_home+"/bin/qmail_run");
		char prog[] = { qp_send_restart, '\0' };
		char *const args[] = {
				const_cast<char *>(restart.c_str()),
				prog,
				NULL
		};
		int rr = run(args);
		if( ! WIFEXITED(rr) || WEXITSTATUS(rr) ) ret = ::vq::ivq::err_temp;
	
		string dir(conf_home+"/domains/"+split_dom(dom)+'/'+dom);
		if(!rmdirrec(dir)) {
				ret = lr(::vq::ivq::err_wr, dir);
		}
	
		return(ret);
	}
	
	/**
	 * Adds domain to system
	 * \param d valid domain name
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error cqmailvq::dom_add(const char *d)
	{
		assert_auth();
		string dom(lower(d));
		if(!dom_val(dom)) return lr(::vq::ivq::err_dom_inv, dom);
	
		if(auth->dom_add(dom.c_str())) { 
				return lr(::vq::ivq::err_auth, auth->err_report()); 
		}
	
		string dom_add_dir(conf_home+"/domains/"+split_dom(dom)+'/'+dom);
		if(!mkdirhier(dom_add_dir.c_str(), 
			ac_vq_mode.val_int(), getuid(), getgid())) {
				auth->dom_rm(dom.c_str());
				return lr(::vq::ivq::err_mkdir, dom);
		}
	
		/* dot file with a call to deliver */
		string dotfile(dom_add_dir+"/.qmail-default");
		if(!dumpstring( dotfile, (string)"|"+conf_home+"/bin/deliver\n") ) {
				rmdirrec(dom_add_dir); 
				auth->dom_rm(dom.c_str());
				return lr(::vq::ivq::err_wr, dotfile);
		}
	
		if( virt_add(dom,dom) ) {
				rmdirrec(dom_add_dir);
				auth->dom_rm(dom.c_str());
				return lastret;
		}
			
		if(locals_rm(dom)) {
				rmdirrec(dom_add_dir); 
				auth->dom_rm(dom.c_str());
				virt_rm(dom);
				return lastret; 
		}
	
		switch(rcpt_add(dom)) {
		case ::vq::ivq::err_no: break;
		case ::vq::ivq::err_over:
				if(morercpt_add(dom) == ::vq::ivq::err_no) break;
		default:
				lastret_blkd=true;
				rmdirrec(dom_add_dir); 
				auth->dom_rm(dom.c_str());
				virt_rm(dom);
				lastret_blkd=false;
				return lastret; 
		}
		if(assign_add(dom)) {
				lastret_blkd=true;
				rmdirrec(dom_add_dir); 
				auth->dom_rm(dom.c_str());
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
				auth->dom_rm(dom.c_str());
				virt_rm(dom);
				lastret_blkd=false;
				return lastret;
		}
	
		return lr(::vq::ivq::err_no, "");
	}

	/**
	 * Add alias for domain
	 * \param d domain
	 * \param a alias
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error cqmailvq::dom_alias_add(const char *d, const char *a)
	{
		string dom(lower(d)), ali(lower(a));
		if( virt_add(ali,dom) )
				return lastret;
		
		switch( rcpt_add(ali) ) {
		case ::vq::ivq::err_no: break;
		case ::vq::ivq::err_over:
				if(morercpt_add(dom) == ::vq::ivq::err_no) break;
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
				return lr(::vq::ivq::err_exec, restart);
		}
		return lr(::vq::ivq::err_no, "");
	}
	
	/**
	 * Adds IP address for domain
	 * \param d domain
	 * \param i IP
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error cqmailvq::dom_ip_add(const char *d, const char *i)
	{
		assert_auth();
		if(auth->dom_ip_add(lower(d).c_str(),lower(i).c_str()))
				return lr(::vq::ivq::err_auth, auth->err_report());
		return lr(::vq::ivq::err_no, "");
	}
	
	/**
	 * Remove IP address of domain
	 * \param d domain's name
	 * \param i IP
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error cqmailvq::dom_ip_rm(const char *d, const char *i)
	{
		assert_auth();
		if(auth->dom_ip_rm(lower(d).c_str(),lower(i).c_str()))
				return lr(::vq::ivq::err_auth, auth->err_report());
		return lr(::vq::ivq::err_no, "");
	}
	
	/**
	 * Remove all IP addresses asociated with domain
	 * \param d domain
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error cqmailvq::dom_ip_rm_all(const char *d)
	{
		assert_auth();
		if(auth->dom_ip_rm_all(lower(d).c_str()))
				return lr(::vq::ivq::err_auth, auth->err_report());
		return lr(::vq::ivq::err_no, "");
	}
	
	/**
	 * Lists IP addresses asociated with domain
	 * \param d domain
	 * \param ips set to list of IP addresses
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error cqmailvq::dom_ip_ls(const char *d, vector<string> &ips)
	{
		assert_auth();
		if(auth->dom_ip_ls(lower(d).c_str(),ips))
				return lr(::vq::ivq::err_auth, auth->err_report());
		return lr(::vq::ivq::err_no, "");
	}
	
	/**
	 * Lists all domains which have asociated IP addresses
	 * \param doms list of domains
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error cqmailvq::dom_ip_ls_dom(vector<string> &doms)
	{
		assert_auth();
		if(auth->dom_ip_ls_dom(doms.c_str()))
				return lr(::vq::ivq::err_auth, auth->err_report());
		return lr(::vq::ivq::err_no, "");
	}
#endif
	
	/**
	 * Lock deliveries to specified domain
	 * \return 0 on success
	 */
	cqmailvq::error cqmailvq::dom_del_lock( const std::string & dom ) {
		struct stat st;
		if(stat(dom.c_str(), &st)) return lr(::vq::ivq::err_stat, dom);
		if(! S_ISDIR(st.st_mode)) return lr(::vq::ivq::err_notdir, dom);
		if( chmod(dom.c_str(), (st.st_mode & 07777) | 01000) )
				return lr(::vq::ivq::err_chmod, dom );
		return lr(::vq::ivq::err_no, "");
	}
	
	/**
	 * \brief unlock delivieries to specified domain
	 * \return 0 on success
	 */
	cqmailvq::error cqmailvq::dom_del_unlock( const std::string & dom ) {
		struct stat st;
		if(stat(dom.c_str(), &st)) return lr(::vq::ivq::err_stat, dom);
		if(! S_ISDIR(st.st_mode)) return lr(::vq::ivq::err_notdir, dom);
		if(chmod(dom.c_str(), (st.st_mode & 07777) & ~01000))
				return lr(::vq::ivq::err_chmod, dom );
		return lr(::vq::ivq::err_no, "");
	}
	
	/**
	 * Create path for specified domain (in vq directory)
	 * \param dom domain
	 * \return path (ie. /var/vq/domains/pl/org/test/test.org.pl)
	 */
	string cqmailvq::dompath(const std::string &dom) {
		return this->home+"/domains/"+split_dom(dom, this->dom_split)+dom;
	}
	
} // namespace POA_vq

