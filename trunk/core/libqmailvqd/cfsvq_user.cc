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
 * Add user
 * \param u user's name
 * \param d domain's name
 * \param p password
 * \param flags if (flags & 1) != 0 then don't check whether login is banned
 */
uint8_t cfsvq::user_add( const string & u, const string &d, const string & p, 
				uint8_t flags )
{
	assert_auth();
	string user(lower(u)), dom(lower(d));

	string domdir(conf_home+"/domains/"+split_dom(dom)+'/'+dom+'/');
	string spuser(split_user(user));
	string user_add_dir(domdir + spuser + '/'+ u);

	/* check wheter domain has default quota for users */
#warning Quota setting removed temporarily	
#if 0	
	quota_value qm=0;
	char ret;
	switch((ret=qt_def_get(dom, qm))) {
	case err_no: break;
	case err_open: if(err_sys() == ENOENT) break;
	return lastret;
	}
#endif	

	if(!mkdirhier(user_add_dir.c_str(), ac_vq_mode.val_int(),
				ac_vq_uid.val_int(), ac_vq_gid.val_int())) 
			return lr(err_mkdir, user_add_dir);
	
	if(maildir_make(user_add_dir+"/Maildir")) {
			rmdirrec(user_add_dir);
			return lr(err_mkdir, user_add_dir+"/Maildir");
	}
#if 0	
	if( qm && (ret=qt_set(dom, user, qm)) != err_no ) {
			rmdirrec(user_add_dir);
			return lastret;
	}
#endif
	if( auth->user_add(u,d,p,flags) ) {
			rmdirrec(user_add_dir);
			return lr(err_auth, auth->err_report());
	}
	string dotuser = dotfile(dom, user, "");
	if( ! dumpstring(dotuser, (string)"./"+user+"/Maildir/\n" )
	   || chown(dotuser.c_str(), ac_vq_uid.val_int(), ac_vq_gid.val_int())
	   || chmod(dotuser.c_str(), ac_vq_fmode.val_int()) ) {
			lastret_blkd = true;
			auth->user_rm(user,dom);
			rmdirrec(user_add_dir);
			lastret_blkd = false;
			return lr(err_wr, dotuser);
	}
	return lr(err_no, "");
}

/**
 * Remove user
 * \param u user
 * \param d domain
 * \return err_no on success
 */
uint8_t cfsvq::user_rm(const string &u,const string &d)
{
	assert_auth();

	string user(lower(u)), dom(lower(d));
	if( auth->user_rm(u,d) ) return lr(err_auth, auth->err_report());

	string dir = conf_home+"/domains/"+split_dom(dom)+'/'+dom+'/'
			+split_user(user) + '/';
	
	ostringstream dir_mv;
	struct timeval time_mv;
	gettimeofday(&time_mv, NULL);
	dir_mv<<conf_home<<"/deleted/@"<<time_mv.tv_sec
			<<'.'<<time_mv.tv_usec<<'.'<<user<<'@'<<dom;

	if(rename((dir+user).c_str(), dir_mv.str().c_str())) 
			return lr(err_ren, dir+user);
	
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
	} else return lr(err_rd, dir);
	
	return lr(err_no, "");
}

/**
 * Change password.
 * \param u user
 * \param d domain
 * \param p new password
 * \return err_no on success
 */
uint8_t cfsvq::user_pass(const string &u,const string &d,const string &p)
{
	assert_auth();
	if(auth->user_pass(lower(u),lower(d),p))
			return lr(err_auth, auth->err_report());
	return lr(err_no, "");
}
/**
 * do user authorization, retrieve user's info
 * \param ai (ai.user should be set to user's name, ai.dom should be set to user's domain)
 * \return 0 true if information was retrieved successful
 */
uint8_t cfsvq::user_auth(auth_info & ai)
{
	assert_auth();
	if( ai.user.empty() )
			return lr(err_user_inv, "it's empty");
			
	if( ai.dom.empty() ) 
			return lr(err_dom_inv, "it's empty");
	
	ai.user=lower(ai.user);
	ai.dom=lower(ai.dom);
	switch( auth->user_auth(ai) ) {
	case err_no: 
			ai.dir = conf_home+"/domains/"+split_dom(ai.dom)+'/'+ai.dom+'/'
					+split_user(ai.user)+'/'+ai.user;
			return lr(err_no, "");
	case err_user_nf: 
			return lr(err_user_nf, ai.user+"@"+ai.dom);
	}
	return lr(err_auth, auth->err_report());
}

/**
 * Checks whether user exists.
 * \param dom domain
 * \param user user
 * \return err_no if user exists
 */
uint8_t cfsvq::user_ex( const string &dom, const string &user ) {
	assert_auth();
	return lr(auth->user_ex(dom, user), user+'@'+dom);
}
