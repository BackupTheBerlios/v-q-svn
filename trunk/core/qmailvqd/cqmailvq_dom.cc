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
#include "qmail_progs.hpp"

#include <pfstream.hpp>
#include <sys.hpp>
#include <text.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <signal.h>

#include <sstream>
#include <iomanip>
#include <memory>
#include <cerrno>
#include <algorithm>

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
	cqmailvq::error * cqmailvq::dom_alias_rm( const char * a )
	{
		auto_ptr<error> ret;
		string alias(lower(a));
		ret.reset( assign_ex(alias) );
		if( ret->ec != ::vq::ivq::err_no ) return ret.release();
		return virt_rm(alias);
	}
#endif // if 0

	/**
	 * Remove domain. It removes domain from qmail control file, authorization
	 * then from filesystem. It ignores errors (so it may not remove something).
	 * \return last cqmailvq::error code returned by any called function. So only ::vq::ivq::err_no means that everything was removed.
	 */
	cqmailvq::error * cqmailvq::dom_rm(const char *d)
	{
		if( !d ) throw ::vq::null_error(__FILE__, __LINE__);

		assert_auth();
		string dom(lower(d));
		auto_ptr<error> ret;
		
		ret.reset(rcpt_rm(dom));
		ret.reset(morercpt_rm(dom));
		ret.reset(virt_rm(dom));
		ret.reset(assign_rm(dom));
	
		if(auth->dom_rm(dom.c_str())) 
			ret.reset(lr(::vq::ivq::err_auth, ""));
	
		string restart(home+"/bin/qmail_run");
		char prog[] = { qp_send_restart, '\0' };
		char *const args[] = {
				const_cast<char *>(restart.c_str()),
				prog,
				NULL
		};
		int rr = run(args);
		if( ! WIFEXITED(rr) || WEXITSTATUS(rr) ) 
				ret.reset(lr(::vq::ivq::err_temp, "wrong exit value"));
	
		string dir(home+"/domains/"+split_dom(dom, dom_split)+'/'+dom);
		if(!rmdirrec(dir)) {
				ret.reset(lr(::vq::ivq::err_wr, dir));
		}
		return ret.release();
	}

	/**
	 * Adds domain to system
	 * \param d valid domain name
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::dom_add(const char *d)
	{
		if( ! d ) throw ::vq::null_error(__FILE__, __LINE__);
		assert_auth();

		string dom(lower(d));
		auto_ptr<error> ret;
		ret.reset( dom_val(dom.c_str()) );
		if( ::vq::ivq::err_no != ret->ec ) 
				return lr(::vq::ivq::err_dom_inv, dom);
	
		CORBA::String_var did;
		if( ::vq::iauth::err_no != auth->dom_add(dom.c_str(), did) ) { 
				return lr(::vq::ivq::err_auth, ""); 
		}
	
		string dom_add_dir(this->home+"/domains/"
			+split_dom(dom, this->dom_split)+'/'+dom);

		if(!mkdirhier(dom_add_dir.c_str(), 
			this->dmode, this->uid, this->gid)) {
				auth->dom_rm(did);
				return lr(::vq::ivq::err_mkdir, dom);
		}
	
		/* dot file with a call to deliver */
		string dotfile(dom_add_dir+"/.qmail-default");
		if(!dumpstring( dotfile, (string)"|"+this->home+"/bin/deliver\n") ) {
				rmdirrec(dom_add_dir); 
				auth->dom_rm(did);
				return lr(::vq::ivq::err_wr, dotfile);
		}
	
		ret.reset( virt_add(dom, dom) );
		if( ret->ec != ::vq::ivq::err_no ) {
				rmdirrec(dom_add_dir);
				auth->dom_rm(did);
				return ret.release();
		}
		
		ret.reset( locals_rm(dom) );
		if( ::vq::ivq::err_no != ret->ec ) {
				rmdirrec(dom_add_dir); 
				auth->dom_rm(did);
				delete virt_rm(dom);
				return ret.release(); 
		}
	
		ret.reset( rcpt_add(dom.c_str()) );
		switch(ret->ec) {
		case ::vq::ivq::err_no: break;
		case ::vq::ivq::err_over:
				ret.reset(morercpt_add( dom.c_str() ) );
				if( ret->ec == ::vq::ivq::err_no) break;
		default:
				rmdirrec(dom_add_dir); 
				auth->dom_rm(did);
				delete virt_rm(dom);
				return ret.release(); 
		}
		ret.reset(assign_add(dom));
		if( ret->ec != ::vq::ivq::err_no ) {
				rmdirrec(dom_add_dir); 
				auth->dom_rm(did);
				delete virt_rm(dom);
				return ret.release();
		}
	
		string restart(this->home+"/bin/qmail_run");
		char prog[] = { qp_send_restart, '\0' };
		char *const args[] = {
				const_cast<char *>(restart.c_str()),
				prog,
				NULL
		};
		int rr = run(args);
		if( ! WIFEXITED(rr) || WEXITSTATUS(rr) ) {
				rmdirrec(dom_add_dir); 
				auth->dom_rm(did);
				delete virt_rm(dom);
				return lr(::vq::ivq::err_temp, "wrong exit value");
		}
		return lr(::vq::ivq::err_no, "");
	}

#if 0
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
	cqmailvq::error * cqmailvq::dom_del_lock( const std::string & dom ) {
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
	cqmailvq::error * cqmailvq::dom_del_unlock( const std::string & dom ) {
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

	/**
	 * Returns id. number of domain
	 */
	cqmailvq::error* cqmailvq::dom_id( const char* dom, 
			CORBA::String_out dom_id ) {
		if( ! dom )	throw ::vq::null_error(__FILE__, __LINE__);
		if( auth->dom_id(dom, dom_id) != ::vq::iauth::err_no )
				return lr(::vq::ivq::err_auth, "");
		return lr(::vq::ivq::err_no, "");
	}

	/**
	 * Returns name of domain with given id. number
	 */
	cqmailvq::error* cqmailvq::dom_name( const char* dom_id, 
			CORBA::String_out dom_name ) {
		if( ! dom_id ) throw ::vq::null_error(__FILE__, __LINE__);
		if( auth->dom_name(dom_id, dom_name) != ::vq::iauth::err_no )
				return lr(::vq::ivq::err_auth, "");
		return lr(::vq::ivq::err_no, "");
	}

} // namespace POA_vq

