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

#include <boost/lexical_cast.hpp>

#include <sys/time.h>
#include <unistd.h>

#include <sstream>
#include <cerrno>

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

		if(!ai.login || !ai.dir)
				throw ::vq::null_error(__FILE__, __LINE__);

		string user(lower(static_cast<const char *>(ai.login)));
		string spuser(this->conf.paths.user_root_path(
				boost::lexical_cast<std::string>(ai.id_domain), user));
		string user_add_dir(ai.dir);
		string user_root_dir = this->conf.paths.user_root_path(
				boost::lexical_cast<std::string>(ai.id_domain), user);

		if( user_add_dir.empty() ) {
			user_add_dir = this->conf.paths.user_dir_path(
				boost::lexical_cast<std::string>(ai.id_domain), user);
		}
	
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
		if(!sys::mkdirhier(user_root_dir.c_str(), this->conf.dmode, 
					this->conf.uid, this->conf.gid)
			&& EEXIST != errno ) 
				return lr( ::vq::ivq::err_mkdir, user_root_dir );

		if(!sys::mkdirhier(user_add_dir.c_str(), this->conf.dmode, 
					this->conf.uid, this->conf.gid)) 
				return lr( EEXIST == errno 
					? ::vq::ivq::err_exists : ::vq::ivq::err_mkdir, user_add_dir);
	
		auto_ptr<error> ret;
		ret.reset(maildir_make(this->conf.paths.user_md_path(user_add_dir)));
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
		string dotuser(dotfile(boost::lexical_cast<std::string>(ai.id_domain), 
				user, ""));
		if( ! sys::dumpstring(dotuser, 
				( '\0' == *ai.dir ? this->conf.paths.user_md_subpath(user) 
				: this->conf.paths.user_md_path(user_add_dir) ) +"\n") ) {

				delete auth->user_rm(ai.id_domain, user.c_str());
				sys::rmdirrec(user_add_dir);
				return lr(::vq::ivq::err_wr, dotuser);
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Modify user. Login and domain are never changed. Password is changed
	 * only if password == true. Directory changes are not
	 * implemented at this time.
	 * \param ui includes information about user
	 * \param password change also password
	 * \param home change also home
	 */
	cqmailvq::error * cqmailvq::user_rep( const user_info & ai, 
			CORBA::Boolean password, CORBA::Boolean home ) std_try {

		if(!ai.login || !ai.dir)
				throw ::vq::null_error(__FILE__, __LINE__);

		string login(lower(static_cast<const char *>(ai.login)));

		user_info ui;
		ui.id_domain = ai.id_domain;
		ui.login = login.c_str();
		auto_ptr<error> ret;
		if( home ) {
			ret.reset(this->user_get(ui)); 
			if( ::vq::ivq::err_no != ret->ec )
				return ret.release();
			if( strcmp(ai.dir, ui.dir) )
				return lr(::vq::ivq::err_func_ni, "");
		}
	
		ret.reset(auth->user_rep(ai, password, home));
		if( ::vq::ivq::err_no != ret->ec ) {
				return ret.release();
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 * Remove user
	 * \param dom_id domain's id
	 * \param login
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::user_rm( id_type dom_id, 
			const char *_login) std_try {

		if( ! _login ) 
				throw ::vq::null_error(__FILE__, __LINE__);
		std::string login(lower(static_cast<const char *>(_login)));
		auto_ptr<error> ret;
		user_info ui;
		ui.id_domain = dom_id;
		ui.login = login.c_str();
		if( this->conf.backup_deleted > 0 ) {
				ret.reset(this->user_get(ui)); 
				if( ::vq::ivq::err_no != ret->ec )
						return ret.release();
		}

		ret.reset(auth->user_rm(dom_id, login.c_str()));
		if( ::vq::ivq::err_no != ret->ec ) 
				return ret.release();
	
		string dir(this->conf.paths.user_root_path(
			boost::lexical_cast<std::string>(dom_id), login)+'/');
		
		replace(login.begin(),login.end(),'.',':');
		sys::cdir_ptr dotdir(opendir(dir.c_str()));
		if( dotdir.get() ) {
				struct dirent *de;
				char * name;
				string::size_type loginl = login.length(); // +6 = .qmail-
				while( (de=readdir(dotdir.get())) ) {
						name = de->d_name;
						if( _D_EXACT_NAMLEN(de) < 7
						  || _D_EXACT_NAMLEN(de)-7 < loginl 
						  || *(name++) != '.' || *(name++) != 'q' 
						  || *(name++) != 'm'
						  || *(name++) != 'a' || *(name++) != 'i'
						  || *(name++) != 'l' || *(name++) != '-' ) continue;
						if( name == login 
							|| ( name[loginl] == '-'
							   && strncmp(name+loginl,login.c_str(),loginl) ) )
								unlink((dir+de->d_name).c_str());
				}
		} else return lr(::vq::ivq::err_rd, dir);

		if( this->conf.backup_deleted > 0 ) {
				ostringstream dir_mv;
				struct timeval time_mv;
				gettimeofday(&time_mv, NULL);
				dir_mv<<this->conf.deleted<<"/@"<<time_mv.tv_sec
						<<'.'<<time_mv.tv_usec<<'.'<<login<<'@'
						<<boost::lexical_cast<std::string>(dom_id);
	
				if(rename(ui.dir, dir_mv.str().c_str())) 
						return lr(::vq::ivq::err_ren, ui.dir);
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Change password.
	 * \param user_id user
	 * \param dom_id domain
	 * \param p new password
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::user_pass(id_type dom_id,
			const char *login, const char * pass ) std_try {
	
		if(!login || !pass)
				throw ::vq::null_error(__FILE__, __LINE__);
		
		return auth->user_pass(dom_id, login, pass);
	} std_catch

	/**
	 * do user authorization, retrieve user's info
	 * \param ai (ai.login should be set to user's name, ai.id_domain should be set to user's domain)
	 * \return 0 true if information was retrieved successful
	 */
	cqmailvq::error * cqmailvq::user_get(user_info & ai) std_try {
		if( ! ai.login ) 
				throw ::vq::null_error(__FILE__, __LINE__);

		auto_ptr<error> ret(auth->user_get(ai));
		if( ::vq::ivq::err_no == ret->ec ) {
				user_info_fill_empty(ai);
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
	cqmailvq::error * cqmailvq::user_ex( id_type dom_id, const char *login ) std_try {
		return auth->user_ex(dom_id, login);
	} std_catch

	/**
	 *
	 */
	cqmailvq::error * cqmailvq::user_ls_by_dom( id_type dom_id, 
			size_type start, size_type cnt, user_info_list_out uis ) std_try {
		uis = new ::vq::ivq::user_info_list;
		auto_ptr<error> ret(auth->user_ls_by_dom( dom_id, start, cnt, uis ));
		if( ::vq::ivq::err_no == ret->ec ) {
				for( CORBA::ULong j=0, k=uis->length(); j<k; ++j ) {
						user_info_fill_empty(uis[j]);
				}
		}
		return ret.release();
	} std_catch

	/**
	 *
	 */
	void cqmailvq::user_info_fill_empty( user_info & ui ) {
		if( '\0' == *ui.dir ) {
				ui.dir = this->conf.paths.user_dir_path(
					boost::lexical_cast<std::string>(ui.id_domain),
					lower(static_cast<const char *>(ui.login))).c_str();
		}
		ui.gid = this->conf.gid;
		ui.uid = this->conf.uid;
	}

	/**
	 *
	 */
	cqmailvq::error * cqmailvq::user_cnt_by_dom( id_type dom_id, size_type &cnt) std_try {
		return auth->user_cnt_by_dom( dom_id, cnt );
	} std_catch

} // namespace POA_vq
