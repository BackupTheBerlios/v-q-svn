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
#include "auto/d_namlen.h"

#include <sys.hpp>
#include <text.hpp>
#include <pfstream.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>

#include <csignal>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cerrno>

namespace POA_vq {

	using namespace std;
	using namespace posix;
	using namespace vq;
	using namespace text;
	
	/**
	@return 1 if user name is valid, 0 otherwise
	*/
	cqmailvq::error * cqmailvq::user_val(const char *ptr)
	{
		if( !ptr )
				throw null_error(__FILE__, __LINE__);
		for( ; *ptr; ptr++ ) {
				if( (*ptr >= 'a' && *ptr <= 'z')
					|| (*ptr >= 'A' && *ptr <= 'Z' )
					|| (*ptr >= '0' && *ptr <= '9' ) )
						continue;
	
				switch(*ptr) {
				case '$': case '%': case '&': case '\'':
				case '*': case '+': case '-': case '/':
				case '=': case '?': case '^': case '_':
				case '`': case '{': case '|': case '}':
				case '~': case '.': case '!': case '#':
						continue;
				default: 
						return lr(::vq::ivq::err_no, "");
				}
		}
		return lr(::vq::ivq::err_user_inv, "illegal chars");
	}

	/**
	 * Add user
	 * \param u user's name
	 * \param d domain's name
	 * \param p password
	 * \param flags if (flags & 1) != 0 then don't check whether login is banned
	 */
#if 0
	cqmailvq::error * cqmailvq::user_add( const auth_info & ai, 
			CORBA::Boolean is_banned )
	{
		assert_auth();
		string user(lower(ai.login));
	
		string domdir(this->home+"/domains/"+split_dom(dom, this->dom_split)+'/'+dom+'/');
		string spuser(split_user(user, this->user_split));
		string user_add_dir(domdir + spuser + '/'+ u);
	
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
	
		if(!mkdirhier(user_add_dir.c_str(), ac_vq_mode.val_int(),
					ac_vq_uid.val_int(), ac_vq_gid.val_int())) 
				return lr(::vq::ivq::err_mkdir, user_add_dir);
		
		if(maildir_make(user_add_dir+"/Maildir")) {
				rmdirrec(user_add_dir);
				return lr(::vq::ivq::err_mkdir, user_add_dir+"/Maildir");
		}
	#if 0	
		if( qm && (ret=qt_set(dom, user, qm)) != ::vq::ivq::err_no ) {
				rmdirrec(user_add_dir);
				return lastret;
		}
	#endif
		if( auth->user_add(u,d,p,flags) ) {
				rmdirrec(user_add_dir);
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		}
		string dotuser = dotfile(dom, user, "");
		if( ! dumpstring(dotuser, (string)"./"+user+"/Maildir/\n" )
		   || chown(dotuser.c_str(), ac_vq_uid.val_int(), ac_vq_gid.val_int())
		   || chmod(dotuser.c_str(), ac_vq_fmode.val_int()) ) {
				lastret_blkd = true;
				auth->user_rm(user,dom);
				rmdirrec(user_add_dir);
				lastret_blkd = false;
				return lr(::vq::ivq::err_wr, dotuser);
		}
		return lr(::vq::ivq::err_no, "");
	}
#endif // if 0
	
	/**
	 * Remove user
	 * \param user_id user
	 * \param dom_id domain's id
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::user_rm(const char * dom_id, 
			const char *user_id)
	{
		if( ! dom_id || ! user_id ) 
				throw ::vq::null_error(__FILE__, __LINE__);

		assert_auth();
	
		if( auth->user_rm(dom_id, user_id) ) 
				return lr(::vq::ivq::err_auth, "");
	
		string dir = this->home+"/domains/"
				+split_dom(dom, this->dom_split)+'/'+dom+'/'
				+split_user(user, this->user_split) + '/';
		
		ostringstream dir_mv;
		struct timeval time_mv;
		gettimeofday(&time_mv, NULL);
		dir_mv<<this->home<<"/deleted/@"<<time_mv.tv_sec
				<<'.'<<time_mv.tv_usec<<'.'<<user<<'@'<<dom;
	
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
	}
	
	/**
	 * Change password.
	 * \param user_id user
	 * \param dom_id domain
	 * \param p new password
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::user_pass(const char * dom_id,
			const char *user_id, const char * p ) {

		if(!dom_id || !user_id || !p)
				throw ::vq::null_error(__FILE__, __LINE__);
		
		assert_auth();

		if(auth->user_pass(dom_id, user_id, p))
				return lr(::vq::ivq::err_auth, "");

		return lr(::vq::ivq::err_no, "");
	}
	/**
	 * do user authorization, retrieve user's info
	 * \param ai (ai.user should be set to user's name, ai.dom should be set to user's domain)
	 * \return 0 true if information was retrieved successful
	 */
	cqmailvq::error * cqmailvq::user_auth(auth_info & ai)
	{
		assert_auth();
		if( ai.user.empty() )
				return lr(::vq::ivq::err_user_inv, "it's empty");
				
		if( ai.dom.empty() ) 
				return lr(::vq::ivq::err_dom_inv, "it's empty");
		
		ai.user=lower(ai.user);
		ai.dom=lower(ai.dom);
		switch( auth->user_auth(ai) ) {
		case ::vq::ivq::err_no: 
				ai.dir = this->home+"/domains/"
						+split_dom(ai.dom, this->dom_split)+'/'+ai.dom+'/'
						+split_user(ai.user, this->user_split)+'/'+ai.user;
				return lr(::vq::ivq::err_no, "");
		case ::vq::ivq::err_user_nf: 
				return lr(::vq::ivq::err_user_nf, ai.user+"@"+ai.dom);
		}
		return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
	}
	
	/**
	 * Checks whether user exists.
	 * \param dom domain
	 * \param user user
	 * \return ::vq::ivq::err_no if user exists
	 */
	cqmailvq::error * cqmailvq::user_ex( const string &dom, const string &user ) {
		assert_auth();
		return lr(auth->user_ex(dom, user), user+'@'+dom);
	}

} // namespace POA_vq
