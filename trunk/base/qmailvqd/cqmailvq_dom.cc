/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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
#include "cqmailvq_common.hpp"

#include <pfstream.hpp>
#include <sys.hpp>
#include <text.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

	/**
	 * Remove domain. It removes domain from qmail control file, authorization
	 * then from filesystem. It ignores errors (so it may not remove something).
	 * \return last cqmailvq::error code returned by any called function. So only ::vq::ivq::err_no means that everything was removed.
	 */
	cqmailvq::error * cqmailvq::dom_rm(const char *dom_id) std_try {
		auto_ptr<error> ret;
		std::string dom;
		
		{
			CORBA::String_var _dom;
			ret.reset(dom_name(dom_id, _dom));
			if( ret->ec != ::vq::ivq::err_no )
					return ret.release();
			dom = static_cast<const char *>(_dom);
		}
	
		ret.reset(dip_rm_by_dom(dom_id));
		if( ret->ec != ::vq::ivq::err_no )
				return ret.release();
		ret.reset(rcpt_rm(dom));
		if( ret->ec != ::vq::ivq::err_no )
				return ret.release();
		ret.reset(morercpt_rm(dom));
		if( ret->ec != ::vq::ivq::err_no )
				return ret.release();
		ret.reset(virt_rm(dom));
		if( ret->ec != ::vq::ivq::err_no )
				return ret.release();
		ret.reset(assign_rm(dom_id));
		if( ret->ec != ::vq::ivq::err_no )
				return ret.release();
	
		ret.reset(auth->dom_rm(dom_id));
		if( ret->ec != ::vq::ivq::err_no )
				return ret.release();
	
		string restart(home+"/bin/qmail-send-restart");
		char *const args[] = {
				const_cast<char *>(restart.c_str()),
				NULL
		};
		int rr = run(args);
		if( ! WIFEXITED(rr) || WEXITSTATUS(rr) ) {
				return lr(::vq::ivq::err_temp, "wrong exit value");
		}

		string dir(paths.dom_path(dom_id));
		if(!rmdirrec(dir) && errno != ENOENT) {
				return lr(::vq::ivq::err_unlink, dir);
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 * Adds domain to system
	 * \param d valid domain name
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::dom_add(const char *d, CORBA::String_out did)
	std_try {
		if( ! d ) throw ::vq::null_error(__FILE__, __LINE__);

		string dom(lower(d));
		auto_ptr<error> ret;
		ret.reset( dom_val(dom.c_str()) );
		if( ::vq::ivq::err_no != ret->ec ) 
				return ret.release();
	
		ret.reset(auth->dom_add(dom.c_str(), did));
		if( ::vq::ivq::err_no != ret->ec ) { 
				return ret.release();
		}
	
		string dom_add_dir(paths.dom_path(static_cast<const char *>(did)));

		if(!mkdirhier(dom_add_dir.c_str(), 
			this->dmode, this->uid, this->gid)) {
				delete auth->dom_rm(did);
				return lr(::vq::ivq::err_mkdir, dom_add_dir);
		}
	
		/* dot file with a call to deliver */
		string dotfile(dom_add_dir+"/.qmail-default");
		if(!dumpstring( dotfile, (string)"|"+this->home+"/bin/deliver\n") ) {
				rmdirrec(dom_add_dir); 
				delete auth->dom_rm(did);
				return lr(::vq::ivq::err_wr, dotfile);
		}
	
		ret.reset( virt_add(dom, static_cast<const char *>(did)) );
		if( ret->ec != ::vq::ivq::err_no ) {
				rmdirrec(dom_add_dir);
				delete auth->dom_rm(did);
				return ret.release();
		}
		
		ret.reset( locals_rm(dom) );
		if( ::vq::ivq::err_no != ret->ec ) {
				rmdirrec(dom_add_dir); 
				delete auth->dom_rm(did);
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
				delete auth->dom_rm(did);
				delete virt_rm(dom);
				return ret.release(); 
		}
		ret.reset(assign_add(static_cast<const char *>(did)));
		if( ret->ec != ::vq::ivq::err_no ) {
				rmdirrec(dom_add_dir); 
				delete auth->dom_rm(did);
				delete virt_rm(dom);
				return ret.release();
		}

		ret.reset(send_restart());
		if( ::vq::ivq::err_no != ret->ec ) {
				rmdirrec(dom_add_dir); 
				delete auth->dom_rm(did);
				delete virt_rm(dom);
		}
		return ret.release();
	} std_catch

	/**
	 *
	 */
	cqmailvq::error * cqmailvq::dom_ls( domain_info_list_out dis ) std_try {
		return auth->dom_ls( dis );
	} std_catch

	/**
	 *
	 */
	cqmailvq::error * cqmailvq::send_restart() std_try {
		string restart(this->home+"/bin/qmail-send-restart");
		char *const args[] = {
				const_cast<char *>(restart.c_str()),
				NULL
		};
		int rr = run(args);
		if( ! WIFEXITED(rr) || WEXITSTATUS(rr) ) {
				return lr(::vq::ivq::err_exec, restart);
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 * Remove aliass.
	 * \param a alias name
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::da_rm( const char * ali ) std_try {

		if( ! ali )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		string alias(text::lower(ali));
		auto_ptr<error> ret( assign_ex(alias) );
		if( ::vq::ivq::err_no != ret->ec ) return ret.release();
		ret.reset(auth->da_rm( alias.c_str() ));
		if( ::vq::ivq::err_no != ret->ec ) return ret.release();
		
		ret.reset(virt_rm(alias));
		if( ::vq::ivq::err_no != ret->ec ) return ret.release();

		ret.reset(send_restart());
		return ret.release();
	} std_catch

	/**
	 * Add alias for domain
	 * \param d domain
	 * \param a alias
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::da_add(const char *dom_id, 
			const char *ali) std_try {

		if( ! dom_id || ! ali )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		string alias(text::lower(ali));
		auto_ptr<error> ret( virt_add(alias, static_cast<const char *>(dom_id)) );
		if( ::vq::ivq::err_no != ret->ec )
				return ret.release();
		
		ret.reset( rcpt_add(alias) );
		switch( ret->ec ) {
		case ::vq::ivq::err_no: break;
		case ::vq::ivq::err_over:
				ret.reset(morercpt_add(alias));
				if( ::vq::ivq::err_no == ret->ec ) break;
		default:
				delete virt_rm(alias);
				return ret.release();
		}

		ret.reset(auth->da_add(dom_id, alias.c_str()));
		if( ::vq::ivq::err_no != ret->ec ) {
				return ret.release();
		}
	
		ret.reset(send_restart());
		if( ::vq::ivq::err_no != ret->ec ) {
				delete virt_rm(alias);
		}
		return ret.release();
	} std_catch

	/**
	 *
	 */
	cqmailvq::error * cqmailvq::da_ls_by_dom( const char * dom_id,
			string_list_out alis ) std_try {

		if( ! dom_id )
				throw ::vq::null_error(__FILE__, __LINE__);
		return auth->da_ls_by_dom( dom_id, alis );
	} std_catch

	/**
	 * Remove alias.
	 * \param a alias name
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::dip_rm( const char * ali ) std_try {

		if( ! ali )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		string alias(text::lower(ali));
		auto_ptr<error> ret(auth->dip_rm( alias.c_str() ));
		if( ::vq::ivq::err_no != ret->ec ) return ret.release();
		
		ret.reset(moreipme_rm(alias));
		if( ::vq::ivq::err_no != ret->ec ) return ret.release();
		return ret.release();
	} std_catch

	/**
	 * Remove aliases for a domain.
	 * \param a alias name
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::dip_rm_by_dom( const char * dom_id ) std_try {

		if( ! dom_id )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		string_list_var alis;
		auto_ptr<error> ret(auth->dip_ls_by_dom( dom_id, alis ));
		if( ::vq::ivq::err_no != ret->ec ) return ret.release();
		CORBA::ULong i, s = alis->length();
		if( ! s ) return lr(::vq::ivq::err_no, "");
		for( i=0; i<s; ++i ) {
				ret.reset(this->dip_rm(alis[i]));
				if( ::vq::ivq::err_no != ret->ec ) return ret.release();
		}
		return ret.release();
	} std_catch

	/**
	 * Add alias for domain
	 * \param d domain
	 * \param a alias
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::dip_add(const char *dom_id, 
			const char *ali) std_try {

		if( ! dom_id || ! ali )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		in_addr addr;
		in6_addr addr6;
		if( inet_aton(ali ? ali : "", &addr) != 1
			&& inet_pton(AF_INET6, ali ? ali : "", &addr6) != 1 )
				return lr(::vq::ivq::err_dom_inv, ali);

		string alias(text::lower(ali));
		
		auto_ptr<error> ret(moreipme_add(alias));
		if( ::vq::ivq::err_no != ret->ec ) return ret.release();

		ret.reset(auth->dip_add(dom_id, alias.c_str()));
		if( ::vq::ivq::err_no != ret->ec ) {
				return ret.release();
		}
		return ret.release();
	} std_catch

	/**
	 *
	 */
	cqmailvq::error * cqmailvq::dip_ls_by_dom( const char * dom_id,
			string_list_out alis ) std_try {

		if( ! dom_id )
				throw ::vq::null_error(__FILE__, __LINE__);
		return auth->dip_ls_by_dom( dom_id, alis );
	} std_catch

	/**
	 * Lock deliveries to specified domain
	 * \return 0 on success
	 */
	cqmailvq::error * cqmailvq::dom_del_lock( const std::string & dom ) std_try {
		struct stat st;
		if(stat(dom.c_str(), &st)) return lr(::vq::ivq::err_stat, dom);
		if(! S_ISDIR(st.st_mode)) return lr(::vq::ivq::err_notdir, dom);
		if( chmod(dom.c_str(), (st.st_mode & 07777) | 01000) )
				return lr(::vq::ivq::err_chmod, dom );
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * \brief unlock delivieries to specified domain
	 * \return 0 on success
	 */
	cqmailvq::error * cqmailvq::dom_del_unlock( const std::string & dom ) std_try {
		struct stat st;
		if(stat(dom.c_str(), &st)) return lr(::vq::ivq::err_stat, dom);
		if(! S_ISDIR(st.st_mode)) return lr(::vq::ivq::err_notdir, dom);
		if(chmod(dom.c_str(), (st.st_mode & 07777) & ~01000))
				return lr(::vq::ivq::err_chmod, dom );
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Returns id. number of domain
	 */
	cqmailvq::error* cqmailvq::dom_id( const char* dom, 
			CORBA::String_out dom_id ) std_try {
		if( ! dom )	throw ::vq::null_error(__FILE__, __LINE__);
		auto_ptr<error> ret(auth->dom_id(dom, dom_id)); 
		if( ret->ec != ::vq::ivq::err_no )
				return ret.release();
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 * Returns name of domain with given id. number
	 */
	cqmailvq::error* cqmailvq::dom_name( const char* dom_id, 
			CORBA::String_out dom_name ) std_try {
		if( ! dom_id ) throw ::vq::null_error(__FILE__, __LINE__);
		auto_ptr<error> ret(auth->dom_name(dom_id, dom_name));
		if( ret->ec != ::vq::ivq::err_no )
				return ret.release();
		return lr(::vq::ivq::err_no, "");
	} std_catch

} // namespace POA_vq

