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
#include "auto/d_namlen.h"

#include <sys.hpp>
#include <text.hpp>
#include <pfstream.hpp>

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>

#include <csignal>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cerrno>
#include <memory>

namespace POA_vq {

	using namespace std;
	using namespace posix;
	using namespace vq;
	using namespace text;
	
	/**
	@return 1 if user name is valid, 0 otherwise
	*/
	cqmailvq::error * cqmailvq::user_val(const char *ptr) std_try {
		if( !ptr )
				throw ::vq::null_error(__FILE__, __LINE__);
		for( ; *ptr; ptr++ ) {
				if( (*ptr >= 'a' && *ptr <= 'z')
					|| (*ptr >= 'A' && *ptr <= 'Z' )
					|| (*ptr >= '0' && *ptr <= '9' ) )
						continue;

#warning check following characters in RFC
				switch(*ptr) {
				case '$': case '%': case '&': case '\'':
				case '*': case '+': case '-': case '/':
				case '=': case '?': case '^': case '_':
				case '`': case '{': case '|': case '}':
				case '~': case '.': case '!': case '#':
						continue;
				default: 
						return lr(::vq::ivq::err_user_inv, "illegal chars");
				}
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch


	/**
	 * Add user
	 * \param ai includes information about user
	 * \param flags if (flags & 1) != 0 then don't check whether login is banned
	 */
	cqmailvq::error * cqmailvq::user_add( const user_info & ai, 
			CORBA::Boolean is_banned ) std_try {

		if(!ai.login || !ai.id_domain)
				throw ::vq::null_error(__FILE__, __LINE__);

		string user(lower(static_cast<const char *>(ai.login)));
		string spuser(paths.user_root_path(static_cast<const char *>(ai.id_domain), 
				user));
		string user_add_dir(paths.user_dir_path(static_cast<const char *>(ai.id_domain), 
				user));
	
		/* check wheter domain has default quota for users */
	#warning Quota setting removed temporarily	
	#if 0	
		quota_value qm=0;
		char ret;
		switch((ret=qt_def_get(dom, qm))) {
		case ::vq::ivq::err_no: break;
		case ::vq::ivq::err_open: if(::vq::ivq::err_sys() == ENOENT) break;
		return lastret;
		}
	#endif	
	
		if(!sys::mkdirhier(user_add_dir.c_str(), this->dmode, 
					this->uid, this->gid)) 
				return lr( EEXIST == errno 
					? ::vq::ivq::err_exists : ::vq::ivq::err_mkdir, user_add_dir);
	
		auto_ptr<error> ret;
		ret.reset(maildir_make(
			paths.user_md_path(static_cast<const char *>(ai.id_domain), user)));
		if( ::vq::ivq::err_no != ret->ec ) {
				sys::rmdirrec(user_add_dir);
				return ret.release();
		}
	#if 0	
		if( qm && (ret=qt_set(dom, user, qm)) != ::vq::ivq::err_no ) {
				rmdirrec(user_add_dir);
				return lastret;
		}
	#endif
		ret.reset(auth->user_add(ai, is_banned));
		if( ::vq::ivq::err_no != ret->ec ) {
				sys::rmdirrec(user_add_dir);
				return ret.release();
		}
		string dotuser(dotfile(static_cast<const char *>(ai.id_domain), 
				user, ""));
		if( ! sys::dumpstring(dotuser, paths.user_md_subpath(user)+"\n") ) {
				delete auth->user_rm(ai.id_domain, user.c_str());
				sys::rmdirrec(user_add_dir);
				return lr(::vq::ivq::err_wr, dotuser);
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Remove user
	 * \param dom_id domain's id
	 * \param login
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::user_rm(const char * dom_id, 
			const char *_login) std_try {

		if( ! dom_id || ! _login ) 
				throw ::vq::null_error(__FILE__, __LINE__);
		std::string login(lower(static_cast<const char *>(_login)));
		auto_ptr<error> ret;
		ret.reset(auth->user_rm(dom_id, login.c_str()));
		if( ::vq::ivq::err_no != ret->ec ) 
				return ret.release();
	
		string dir(paths.user_root_path(dom_id, login)+'/');
		ostringstream dir_mv;
		struct timeval time_mv;
		gettimeofday(&time_mv, NULL);
		dir_mv<<this->home<<"/deleted/@"<<time_mv.tv_sec
				<<'.'<<time_mv.tv_usec<<'.'<<user<<'@'
				<<static_cast<const char *>(dom_id);
	
		if(rename((dir+user).c_str(), dir_mv.str().c_str())) 
				return lr(::vq::ivq::err_ren, dir+user);
		
		replace(user.begin(),user.end(),'.',':');
		DIR *dotdir = opendir(dir.c_str());
		if( dotdir ) {
				struct dirent *de;
				char * name;
				string::size_type userl = user.length(); // +6 = .qmail-
				while( (de=readdir(dotdir)) ) {
						name = de->d_name;
						if( _D_EXACT_NAMLEN(de) < 7
						  || _D_EXACT_NAMLEN(de)-7 < userl 
						  || *(name++) != '.' || *(name++) != 'q' 
						  || *(name++) != 'm'
						  || *(name++) != 'a' || *(name++) != 'i'
						  || *(name++) != 'l' || *(name++) != '-' ) continue;
						if( name == user 
							|| ( name[userl] == '-'
							   && strncmp(name+userl,user.c_str(),userl) ) )
								unlink((dir+de->d_name).c_str());
				}
				closedir(dotdir);
		} else return lr(::vq::ivq::err_rd, dir);
		
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Change password.
	 * \param user_id user
	 * \param dom_id domain
	 * \param p new password
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::user_pass(const char * dom_id,
			const char *login, const char * pass ) std_try {
	
		if(!dom_id || !login || !pass)
				throw ::vq::null_error(__FILE__, __LINE__);
		
		return auth->user_pass(dom_id, login, pass);
	} std_catch

	/**
	 * do user authorization, retrieve user's info
	 * \param ai (ai.login should be set to user's name, ai.id_domain should be set to user's domain)
	 * \return 0 true if information was retrieved successful
	 */
	cqmailvq::error * cqmailvq::user_get(user_info & ai) std_try {
		if( ! ai.login || ! ai.id_domain ) 
				throw ::vq::null_error(__FILE__, __LINE__);

		auto_ptr<error> ret(auth->user_get(ai));
		if( ::vq::ivq::err_no == ret->ec ) {
				if( '\0' == *ai.dir ) {
						ai.dir = paths.user_dir_path(
							static_cast<const char *>(ai.id_domain),
							lower(static_cast<const char *>(ai.login))).c_str();
				}
				ai.gid = this->gid;
				ai.uid = this->uid;
				return lr(::vq::ivq::err_no, "");
		}
		return ret.release();
	} std_catch
	
	/**
	 * Checks whether user exists.
	 * \param dom_id domain's id.
	 * \param login user
	 * \return ::vq::ivq::err_no if user exists
	 */
	cqmailvq::error * cqmailvq::user_ex( const char * dom_id, 
			const char *login ) std_try {
		return auth->user_ex(dom_id, login);
	} std_catch

} // namespace POA_vq
