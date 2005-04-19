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
#include "qmail_files.hpp"

#include <sys.hpp>

#include <boost/lexical_cast.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <sstream>

namespace POA_vq {

	using namespace std;
	using namespace posix;
	using namespace vq;
	
	const char cqmailvq::tmp_end[] = ".tmp"; //!< prefix for temporary files
	
	/**
	 */
	cqmailvq::cqmailvq( const std::string & h, const std::string &d,
				::vq::iauth_var & auth,
				unsigned s_dom, unsigned s_user,
				mode_t fm, mode_t mm, mode_t dm,
				const std::string & user, uid_t uid, gid_t gid ) 
			: home(h), domains(d),
			fmode(fm), mmode(mm), dmode(dm), user(user), uid(uid), 
			gid(gid), paths(d, s_dom, s_user), auth(auth) std_try {
	} std_catch
	
	/**
	 * Write data to file in a safe maner. It opens file, creates temporary file,
	 * copies data to it (until end of file or specified line was found),
	 * closes data. It stats original file, and change mode and owner of created
	 * file. It writes information about temporary file name into fntmp. Caller
	 * needs to rename it.
	 *
	 * \param fn path to file that is going to be changed
	 * \param mode default mode of created file (if it does not exists)
	 * \param eof_mark if not empty original file is read until line including it
	 * \param item_mark mark of existence
	 * \param item_whole if true match whole line while looking for existence
	 * \param item_add this will be added to file
	 * \param ex this value will be returned if entry is in file
	 * \param fntmp temporary file's name 
	 * 
	 * \return error code, 0 on success, or value of ex if entry was found.
	 */
	cqmailvq::error * cqmailvq::file_add(const string &fn, mode_t mode, 
			const string &eof_mark, const string &item_mark, 
			bool item_whole, const string &item_add, string &fntmp) std_try {
	
		string fnlck(fn+".lock");
		opfstream l(fnlck.c_str());
		if( ! l ) return lr(::vq::ivq::err_open, fnlck);
	
		if( sys::lock_exnb(l.rdbuf()->fd()) ) return lr(::vq::ivq::err_lckd, fnlck);
	
		pfstream in(fn.c_str(),ios::in);
		bool enoent = false;
		if(!in) {
				if( errno != ENOENT ) return lr(::vq::ivq::err_open, fn);
				enoent = true;
		}
	
		fntmp = fn+sys::uniq();
		opfstream tmp(fntmp.c_str());
		if(!tmp) return lr(::vq::ivq::err_wr, fntmp);
	
		if( ! enoent ) {
				string ln;
				while(getline(in,ln) && tmp.good()) {
						if(!eof_mark.empty() && ln==eof_mark) break;
						if( (!item_whole 
							&& ln.substr(0,item_mark.length())==item_mark)
							|| (item_whole && ln == item_mark) ) {
								unlink(fntmp.c_str());
								return lr(::vq::ivq::err_exists, fn);
						}
						tmp << ln << endl;
				}
				if( in.bad() ) {
						unlink(fntmp.c_str());
						return lr(::vq::ivq::err_rd, fn);
				}
		}
		tmp << item_add << endl;
		if(!eof_mark.empty()) 
				tmp<<eof_mark<<endl;
	
		tmp.close();
		if( ! tmp ) {
				unlink(fntmp.c_str());
				return lr(::vq::ivq::err_wr, fntmp);
		}
	
		if( ! enoent ) {
				struct stat st;
				if(!fstat(in.rdbuf()->fd(), &st)) {
						if( chmod(fntmp.c_str(), st.st_mode & 07777 ) )
								return lr(::vq::ivq::err_chmod, fntmp);
				} else 
						return lr(::vq::ivq::err_stat, fn);
		} else if( chmod(fntmp.c_str(), this->fmode) )
				return lr(::vq::ivq::err_chmod, fntmp);
		
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * fork && run specified command
	 * \param args array passed to execv, *args is the command to run
	 * \return value returned from wait
	 */
	int cqmailvq::run(char *const args[]) std_try {
		pid_t pid;
	
		switch((pid=vfork())) {
		case -1: 
				return -1;
		case 0: 
				execv( *args, args );
				_exit(111);
		}
		while( wait(&pid) == -1 && errno == EINTR );
		return pid;
	} std_catch

	/**
	 * Create prefix name that will be put in virtualdomains and users/assign
	 * \return prefix string
	 * \param dom_id domain's id.
	 */
	std::string cqmailvq::virt_prefix( const std::string & dom_id ) const {
		return paths.dom_path(dom_id);
	}

	/**
	 * Create line that will be added to qmail_home/users/assign
	 * \return line string
	 * \param dom_id domain's id.
	 */
	std::string cqmailvq::assign_ln( const std::string & dom_id ) const {
		ostringstream domln;
		domln 
			<< '+' << virt_prefix(dom_id) << "-:" 
			<< this->user << ':' << this->uid << ':' << this->gid << ':'
			<< paths.dom_path(dom_id) << ":-::";
		return domln.str();
	}

	/**
	 * Adds domain to assign file, it creates temporary file 
	 * (name in tmpfs.assign_add) which you should rename to assign on success
	 * \param dom_id domain's id.
	 * \return 0 on success
	 */
	cqmailvq::error * cqmailvq::assign_add( const std::string &dom_id ) std_try {   
		string ln(assign_ln(dom_id));
		string prog(home+"/bin/qmail_assign_add");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				const_cast<char *>(ln.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0:
						return lr(::vq::ivq::err_no, "");
				case 1:
						return lr(::vq::ivq::err_exists, ln);
				}
		}
		return lr(::vq::ivq::err_exec,prog);
	} std_catch
	
	/**
	 * Checks whether domain is in assign, if it's not returns 0, 
	 * if it is returns ::vq::ivq::err_dom_ex, on errors returns something else
	 */
	cqmailvq::error * cqmailvq::assign_ex( const std::string &dom_id ) std_try {
		string prog(home+"/bin/qmail_assign_ex");
		string prefix(virt_prefix(dom_id));
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				const_cast<char *>(prefix.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 1:
						return lr(::vq::ivq::err_no,"");
				case 0:
						return lr(::vq::ivq::err_exists,prog);
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	/**
	 * Adds domain to assign file, it creates temporary file 
	 * (name in tmpfs.assign_add) which you should rename to assign on success
	 * \param dom_id domain's id.
	 * \return 0 on success
	 */
	cqmailvq::error * cqmailvq::assign_rm( const std::string &dom_id ) std_try {   
		string ln(assign_ln(dom_id));
		string prog(home+"/bin/qmail_assign_rm");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				const_cast<char *>(ln.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0: case 1: case 2:
						return lr(::vq::ivq::err_no, "");
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	
	/**
	 * Adds host to rcpthosts, it creates temp. file 
	 * (name in tmpfs.rcpt_add) which you should rename to rcpthosts on success,
	 * if morercpthosts is changed rcpthosts is set to "", tmpfs.morercpt_add is set
	 */
	cqmailvq::error * cqmailvq::rcpt_add(const string &dom) std_try {
		string prog(home+"/bin/qmail_rcpthosts_add");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				const_cast<char *>(dom.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0: case 1:
						return lr(::vq::ivq::err_no, "");
				case 2:
						return lr(::vq::ivq::err_over, prog);
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	
	/**
	 * Remove host to rcpthosts, it creates temp. file 
	 * (name in tmpfs.rcpt_rm) which you should rename to rcpthosts on success
	 */
	cqmailvq::error * cqmailvq::rcpt_rm(const string &dom) std_try {
		return qmail_file_rm( qf_rcpthosts, dom );
	} std_catch

	/**
	 *
	 */
	cqmailvq::error * cqmailvq::qmail_file_rm( qmail_file qf, 
			const string & ln ) std_try {
		char file[] = { qf, '\0' };
		string prog(home+"/bin/qmail_file_rm");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				file,
				const_cast<char *>(ln.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0: case 1: case 2:
						return lr(::vq::ivq::err_no, "");
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	
	/**
	 *
	 */
	cqmailvq::error * cqmailvq::qmail_file_add( qmail_file qf, 
			const string & ln ) std_try {
		char file[] = { qf, '\0' };
		string prog(home+"/bin/qmail_file_add");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				file,
				const_cast<char *>(ln.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0:
						return lr(::vq::ivq::err_no, "");
				case 1:
						return lr(::vq::ivq::err_exists, "");
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch

	/**
	 * Removes domain from locals, sets tmpfs.locals_rm to
	 * name of created file, on success you have to rename it to locals
	 */
	cqmailvq::error * cqmailvq::locals_rm(const string &dom) std_try {
		char file[] = { qf_locals, '\0' };
		string prog(home+"/bin/qmail_file_rm");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				file,
				const_cast<char *>(dom.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0: case 1: case 2:
						return lr(::vq::ivq::err_no, "");
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	
	/**
	 * Remove line from a file. Lock original file, copy content to a temporary
	 * file and return it's name. Caller will rename temporary file to original.
	 * \param fn name of file
	 * \param item line to remove
	 * \param fntmp temporary file name
	 * \return 0 if success
	 */
	cqmailvq::error * cqmailvq::file_rm( const std::string & fn, const std::string & item,
			std::string & fntmp ) std_try {
	
		fntmp = fn+sys::uniq();
		string fnlck(fn+".lock");
	
		opfstream lock(fnlck.c_str());
		if( ! lock ) return lr(::vq::ivq::err_open, fnlck);
	
		if( sys::lock_exnb(lock.rdbuf()->fd()) ) return lr(::vq::ivq::err_lckd, fnlck);
	
		ipfstream in(fn.c_str(),ios::in);
		if(!in) {
				if( errno == ENOENT ) return lr(::vq::ivq::err_no, "");
				return lr(::vq::ivq::err_open, fn);
		}
		opfstream tmp(fntmp.c_str(),ios::trunc);
		if(!tmp) return lr(::vq::ivq::err_open, fntmp);
		
		string ln;
		while( getline(in,ln) && tmp.good() ) {
				if(ln==item) continue;
				tmp<<ln<<endl;
		}
		if(in.bad()) {
				unlink(fntmp.c_str());
				return lr(::vq::ivq::err_rd, fn);
		}
		tmp.close();
		if(! tmp) {
				unlink(fntmp.c_str());
				return lr(::vq::ivq::err_wr, fntmp);
		}
	
		struct stat st;
		if(!fstat(in.rdbuf()->fd(), &st)) {
				if( chmod(fntmp.c_str(), st.st_mode & 07777 ) )
						return lr(::vq::ivq::err_chmod, fntmp);
		} else 
				return lr(::vq::ivq::err_stat, fntmp);
		
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Adds domain to virtualdomains, it creates temp. file (name in
	 * tmpfs.virt_add) which should be renamed to virtualdomains on success
	 */
	cqmailvq::error * cqmailvq::virt_add(const string &dom, const string &al) std_try {
		string ln(dom+':'+virt_prefix(al));
		string prog(home+"/bin/qmail_virtualdomains_add");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				const_cast<char *>(dom.c_str()),
				const_cast<char *>(ln.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0:
						return lr(::vq::ivq::err_no, "");
				case 1:
						return lr(::vq::ivq::err_exists, prog);
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	
	/**
	 * Remove alias from virtualdomains. It does not check whether it's a domain
	 * (if you want to remove only aliases you need to first use assign_ex to
	 * check whether alias is in users/assign, if it is it means it's not an
	 * alias).
	 */
	cqmailvq::error * cqmailvq::virt_rm(const string &ali) std_try {
		string prog(home+"/bin/qmail_virtualdomains_rm");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				const_cast<char *>(ali.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0: case 1: case 2:
						return lr(::vq::ivq::err_no, "");
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	
	/**
	 * Adds domain to morercpthosts
	 */
	cqmailvq::error * cqmailvq::morercpt_add(const string &dom) std_try {
		string prog(home+"/bin/qmail_mrh_add");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				const_cast<char *>(dom.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0: case 1:
						return lr(::vq::ivq::err_no, "");
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	
	/**
	 * Adds IP to moreipme
	 */
	cqmailvq::error * cqmailvq::moreipme_add(const string &ip) std_try {
		return qmail_file_add( qf_moreipme, ip );
	} std_catch

	/**
	 * Removes IP from moreipme
	 */
	cqmailvq::error * cqmailvq::moreipme_rm(const string &ip) std_try {
		return qmail_file_rm( qf_moreipme, ip );
	} std_catch

	/**
	 * Add line to a file. Create temporary file, copy content to it,
	 * add item, return temporary file's name.
	 * \param fn file when we add a line
	 * \param item line to add
	 * \param fntmp temp. file's name
	 * \param beg_at if true add line at begining
	 * \return ::vq::ivq::err_no if success
	 */
	cqmailvq::error * cqmailvq::file_add( const string & fn, const string &item,
			string & fntmp, bool beg_at ) std_try {
	
		fntmp = fn+sys::uniq();
		string fnlck(fn+".lock");
		opfstream lock(fnlck.c_str());
		if( ! lock ) return lr(::vq::ivq::err_open, fnlck);
	
		if( sys::lock_exnb(lock.rdbuf()->fd()) ) return lr(::vq::ivq::err_lckd, fnlck);
	
		ipfstream in(fn.c_str());
		bool enoent = false;
		if(!in) {
				if( errno != ENOENT ) return lr(::vq::ivq::err_open, fn);
				enoent = true;
		}
	
		opfstream tmp(fntmp.c_str(),ios::trunc);
		if(!tmp) return lr(::vq::ivq::err_open, fntmp);
	
		if( ! enoent ) {
				string ln;
				if(beg_at) tmp<<item<<endl;
	
				while( getline(in,ln) && tmp.good() ) {
						if( item == ln ) continue;
						tmp<<ln<<endl;
				}
				if( in.bad() ) {
						unlink(fntmp.c_str());
						return lr(::vq::ivq::err_rd, fn);
				}
		}
		if(!beg_at) tmp<<item<<endl;
		tmp.close();
		if( tmp.bad() ) {
				unlink(fntmp.c_str());
				return lr(::vq::ivq::err_wr, fntmp);
		}
	
		if( ! enoent ) {
				// try to preserve all of file's information, ignore errors
				struct stat st;
				if(!fstat(in.rdbuf()->fd(), &st)) {
						if( chmod( fntmp.c_str(), st.st_mode & 07777 ) )
								return lr(::vq::ivq::err_chmod, fntmp);
				} else
						return lr(::vq::ivq::err_stat, fn);
		} else if( chmod(fntmp.c_str(), this->fmode) )
				return lr(::vq::ivq::err_chmod, fntmp);
		
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Replace first line in file. Create temporary file, copy content to it,
	 * add item, return temporary file's name.
	 * \param fn file when we add a line
	 * \param item line to add
	 * \param fntmp temp. file's name
	 * \return ::vq::ivq::err_no if success
	 */
	cqmailvq::error * cqmailvq::file_first_rep( const string & fn, const string &item,
			string & fntmp ) std_try {
	
		fntmp = fn+sys::uniq();
		string fnlck(fn+".lock");
		opfstream lock(fnlck.c_str());
		if( ! lock ) return lr(::vq::ivq::err_open, fnlck);
	
		if( sys::lock_exnb(lock.rdbuf()->fd()) ) return lr(::vq::ivq::err_lckd, fnlck);
	
		ipfstream in(fn.c_str());
		bool enoent = false;
		if(!in) {
				if( errno != ENOENT ) return lr(::vq::ivq::err_open, fn);
				enoent = true;
		}
	
		opfstream tmp(fntmp.c_str(),ios::trunc);
		if(!tmp) return lr(::vq::ivq::err_open, fntmp);
	
		if( ! enoent ) {
				string ln;
				bool first=true;
				while( getline(in,ln) && tmp.good() ) {
						if( first ) {
								tmp<<item<<endl;
								first = false;
						} else {
								tmp<<ln<<endl;
						}
				}
				if( in.bad() ) {
						unlink(fntmp.c_str());
						return lr(::vq::ivq::err_rd, fn);
				}
		} else {
				tmp<<item<<endl;
		}
		tmp.close();
		if( tmp.bad() ) {
				unlink(fntmp.c_str());
				return lr(::vq::ivq::err_wr, fntmp);
		}
	
		if( ! enoent ) {
				// try to preserve all of file's information, ignore errors
				struct stat st;
				if(!fstat(in.rdbuf()->fd(), &st)) {
						if( chmod( fntmp.c_str(), st.st_mode & 07777 ) )
								return lr(::vq::ivq::err_chmod, fntmp);
				} else
						return lr(::vq::ivq::err_stat, fn);
		} else if( chmod(fntmp.c_str(), this->fmode) )
				return lr(::vq::ivq::err_chmod, fntmp);
		
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Adds domain to morercpthosts, creates temp. file which should
	 * be renamed to morercpthosts, you should also run morercpt_comp.
	 */
	cqmailvq::error * cqmailvq::morercpt_rm(const string &dom) std_try {
		string prog(home+"/bin/qmail_mrh_rm");
		char * const args[] = {
				const_cast<char *>(prog.c_str()),
				const_cast<char *>(dom.c_str()),
				NULL
		};
	
		int ret = run(args);
		if( WIFEXITED(ret) ) {
				switch(WEXITSTATUS(ret)) {
				case 0: case 1: case 2:
						return lr(::vq::ivq::err_no, "");
				}
		}
		return lr(::vq::ivq::err_exec, prog);
	} std_catch
	
	/**
	 * Create maildir 
	 * \param d directory for maildir (new,cur,tmp will be created as subdirectories of it)
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::maildir_make(const string & d) std_try {
		if( mkdir(d.c_str(), this->mmode )
			|| chown(d.c_str(), this->uid, this->gid)) 
				return lr(::vq::ivq::err_temp, d.c_str());
		string dir = d+"/new"; 
		if( mkdir(dir.c_str(), this->mmode ) 
			|| chown(dir.c_str(), this->uid, this->gid)) 
				return lr(::vq::ivq::err_temp, dir);
		dir = d+"/cur";
		if( mkdir(dir.c_str(), this->mmode ) 
			|| chown(dir.c_str(), this->uid, this->gid)) 
				return lr(::vq::ivq::err_temp, dir);
		dir = d+"/tmp";
		if( mkdir(dir.c_str(), this->mmode ) 
			|| chown(dir.c_str(), this->uid, this->gid)) 
				return lr(::vq::ivq::err_temp, dir);
		return lr(::vq::ivq::err_no, dir);
	} std_catch
	
#if 0
	/**
	 * Set current usage of mailbox.
	 * \param d domain
	 * \param u user
	 * \param c value added to current usage
	 */
	cqmailvq::error * cqmailvq::qt_cur_set(const string&d, const string &u, int32_t c) std_try {
	#warning there is nothing here
			return ::vq::ivq::err_no;
	//		return qtf_cur_set(qtfile(d,u), c);
	} std_catch
	
	/**
	 * Set quota for user
	 * \param d domain
	 * \param u user
	 * \param qb bytes limit
	 * \param qf files limit
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::qt_set(const string &d, const string &u, 
			quota_type qb, quota_type qf ) {
	
		string dom(text::lower(d)), user(text::lower(u));
	
		if( ! qb && ! qf ) {
				string fn(user_md_path(dom, user)+"maildirquota");
				if( unlink(fn.c_str()) && errno != ENOENT )
						return lr(::vq::ivq::err_unlink, fn);
				fn = user_md_path(dom, user)+"maildirsize";
				if( unlink(fn.c_str()) && errno != ENOENT )
						return lr(::vq::ivq::err_unlink, fn);
				if( auth->qt_set(dom, user, qb, qf) )
						return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		} else {
				ostringstream qtstr;
				qtstr<<qb<<'S'<<','<<qf<<'C';
				string mdqfn(user_md_path(dom, user)+"maildirquota");
				string mdqtmp, mdstmp;
				string mdsfn(user_md_path(dom, user)+"maildirsize");
				quota_type qbcur, qfcur;
	
				if( auth->qt_get(dom, user, qbcur, qfcur) )
						return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
				
				if( file_first_rep(mdqfn, qtstr.str(), mdqtmp) ) {
						unlink(mdqtmp.c_str());
						return lr(lastret, lr_info);
				}
				if( file_first_rep(mdsfn, qtstr.str(), mdstmp) ) {
						unlink(mdqtmp.c_str());
						unlink(mdstmp.c_str());
						return lr(lastret, lr_info);
				}
				if( auth->qt_set(dom, user, qb, qf) ) {
						unlink(mdqtmp.c_str());
						unlink(mdstmp.c_str());
						return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
				}
				if( rename(mdqtmp.c_str(), mdqfn.c_str()) ) {
						unlink(mdqtmp.c_str());
						unlink(mdstmp.c_str());
						auth->qt_set(dom, user, qbcur, qfcur);
						return lr(::vq::ivq::err_ren, mdqtmp);
				}
				if( rename(mdstmp.c_str(), mdsfn.c_str()) ) {
						unlink(mdstmp.c_str());
						return lr(::vq::ivq::err_ren, mdstmp);
				}
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Get quota for user
	 * \param d domain
	 * \param u user
	 * \param qb bytes limit
	 * \param qf files limit
	 */
	cqmailvq::error * cqmailvq::qt_get(const string &d, const string &u, 
			quota_type &qb, quota_type & qf) {
		if( auth->qt_get(text::lower(d), text::lower(u), qb, qf) )
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Checks whether user is over quota
	 * \see cqmailvq::qtf_over
	 */
	cqmailvq::error * cqmailvq::qt_over(const string &d, const string &u) std_try {
	#warning empty function		
	//	return qtf_over(qtfile(d,u));
		return ::vq::ivq::err_no;
	} std_catch
	
	/**
	 * set default user quota for this domain
	 * \param d domain
	 * \param qb bytes limit
	 * \param qf files limit
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::qt_def_set(const string &d, quota_type qb, quota_type qf ) std_try {
		if( auth->qt_def_set(text::lower(d), qb, qf) )
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * set global default user quota
	 * \param qb bytes limit
	 * \param qf files limit
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::qt_global_def_set(quota_type qb, quota_type qf ) std_try {
		if( auth->qt_global_def_set(qb, qf) )
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * get default user quota for this domain
	 * \param d domain
	 * \param qb bytes limit
	 * \param qf files limit
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::qt_def_get(const string &d, quota_type & qb, quota_type & qf) std_try {
		if( auth->qt_def_get(text::lower(d), qb, qf) )
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * get global default user quota
	 * \param qb bytes limit
	 * \param qf files limit
	 * \return ::vq::ivq::err_no on success
	 */
	cqmailvq::error * cqmailvq::qt_global_def_get(quota_type & qb, quota_type & qf) std_try {
		if( auth->qt_global_def_get(qb, qf) )
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * set user's quota from default domain's quota
	 */
	cqmailvq::error * cqmailvq::qt_def_cp(const string &d, const string &u) std_try {
	#warning empty function
		return ::vq::ivq::err_no;
	/*	cqmailvq::error * ret;
		quota_type qc, qm;
		ret = qtf_get(qtfile_def(d), qm, qc);
		if( ret ) return lr(ret, "");
		return lr(qt_set(d,u,qm), "");*/
	} std_catch
#endif // if 0
	
	/**
	@return 1 on valid domain name, 0 otherwise
	\note I assume specific order of characters in code page.
	*/
	cqmailvq::error * cqmailvq::dom_val(const char * ptr) std_try {
		if( ! ptr )
				throw null_error(__FILE__, __LINE__);

		for( ; *ptr; ptr++ ) {
				if( ! ( ( *ptr >= 'A' && *ptr <= 'Z' )
					|| ( *ptr >= 'a' && *ptr <= 'z' )
					|| ( *ptr >= '0' && *ptr <='9' )
					|| *ptr == '.' || *ptr == '-' ) )
						return lr(::vq::ivq::err_dom_inv, "illegal chars");
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
#if 0
	/**
	 * Parse string into quota values.
	 * \param str string including quota values separated by coma, i.e. 12C,34S
	 * means 12 messages, 34 bytes. Some values may be missing and it's not error.
	 * \param bytes quota_value, if not present will be set to 0
	 * \param files quota_value, if not present will be set to 0
	 * \return ::vq::ivq::err_no if values were correct
	 */
	cqmailvq::error * cqmailvq::qt_parse( const string & str, 
			quota_value & bytes, quota_value & files ) {
		
		bytes = files = 0;
		
		quota_value qv;
		istringstream is;
		char ch;
		is.str(str);
		do {
				is>>qv;
				is>>ch;
				if(is.fail()) return lr(::vq::ivq::err_temp,"Invalid quota");
				switch(ch) {
				case 'C': 
						files = qv;
						break;
				case 'S': 
						bytes = qv;
						break;
				default:
						return lr(::vq::ivq::err_temp, "Invalid quota");
				}
				is>>ch;
				if(is.eof()) break;
				if(ch != ',') return lr(::vq::ivq::err_temp,"Invalid quota");
		} while( ! is.eof() && ! is.fail() );
		
		return is.eof() ? lr(::vq::ivq::err_no,"") : lr(::vq::ivq::err_temp,"Invalid quota");
	} std_catch
#endif // if 0

	/**
	 *
	 */
	cqmailvq::error * cqmailvq::lr_wrap(err_code ec, const char *what,
			const char * file, CORBA::ULong line ) std_try {
		auto_ptr<error> err(new error);
		err->ec = ec;
		err->what = CORBA::string_dup(what); // string_dup not really needed
		err->file = CORBA::string_dup(file);
		err->line = line;
		err->auth = 0;
		return err.release();
	} std_catch

} // namespace vq
