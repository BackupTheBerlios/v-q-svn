/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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
#include "qmail_files.h"
#include "uniq.h"

using namespace std;
using namespace posix;
using namespace vq;

const char cfsvq::tmp_end[] = ".tmp"; //!< prefix for temporary files

const char cfsvq::ud_sup[] = "FSMR"; //!< F - forwarding, S - sms notification, M - maildir

cvq * cvq::alloc( ) {
	return new cfsvq();
}

/**
 */
cfsvq::cfsvq() {
	umask(0);
}

/**
 * Write data to file in a safe maner. It opens file, creates temporary file,
 * copies data to it (until end of file or specified line was found),
 * closes data. It stats original file, and change mode and owner of created
 * file. It write information about temporary file name into fntmp. Caller
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
uint8_t cfsvq::file_add(const string &fn, mode_t mode, 
		const string &eof_mark, const string &item_mark, 
		bool item_whole, const string &item_add, uint8_t ex, string &fntmp) {

	string fnlck(fn+".lock");
	opfstream l(fnlck.c_str());
	if( ! l ) return lr(err_open, fnlck);

	if( lock_exnb(l.rdbuf()->fd()) ) return lr(err_lckd, fnlck);

	pfstream in(fn.c_str(),ios::in);
	bool enoent = false;
	if(!in) {
			if( errno != ENOENT ) return lr(err_open, fn);
			enoent = true;
	}

	fntmp = fn+uniq();
	opfstream tmp(fntmp.c_str());
	if(!tmp) return lr(err_wr, fntmp);

	if( ! enoent ) {
			string ln;
			while(getline(in,ln) && tmp.good()) {
					if(!eof_mark.empty() && ln==eof_mark) break;
					if( (!item_whole 
						&& ln.substr(0,item_mark.length())==item_mark)
						|| (item_whole && ln == item_mark) ) {
							unlink(fntmp.c_str());
							return lr(ex, fn);
					}
					tmp << ln << endl;
			}
			if( in.bad() ) {
					unlink(fntmp.c_str());
					return lr(err_rd, fn);
			}
	}
	tmp << item_add << endl;
	if(!eof_mark.empty()) 
			tmp<<eof_mark<<endl;

	tmp.close();
	if( ! tmp ) {
			unlink(fntmp.c_str());
			return lr(err_wr, fntmp);
	}

	if( ! enoent ) {
			struct stat st;
			if(!fstat(in.rdbuf()->fd(), &st)) {
					if( chown(fntmp.c_str(), st.st_uid, st.st_gid) )
							return lr(err_chown, fntmp);
					if( chmod(fntmp.c_str(), st.st_mode & 07777 ) )
							return lr(err_chmod, fntmp);
			} else 
					return lr(err_stat, fn);
	} else if( chmod(fntmp.c_str(), ac_vq_fmode.val_int()) )
			return lr(err_chmod, fntmp);
	
	return lr(err_no, "");
}

/**
 * fork && run specified command
 * \param args array passed to execv, *args is the command to run
 * \return value returned from wait
 */
int cfsvq::run(char *const args[]) {
	pid_t pid;

	switch((pid=vfork())) {
	case -1: 
			return lr(err_temp,"fork failed");
	case 0: 
			execv( *args, args );
			_exit(111);
	}
	while( wait(&pid) == -1 && errno == EINTR );
	return pid;
}

/**
 * Adds domain to assign file, it creates temporary file 
 * (name in tmpfs.assign_add) which you should rename to assign on success
 * \param dom name
 * \return 0 on success
 */
uint8_t cfsvq::assign_add( const string &dom )
{   
	ostringstream domln;
	domln << '+'<< dom << "-:" << ac_vq_user.val_str() << ':' 
		<< ac_vq_uid.val_str() << ':' << ac_vq_gid.val_str() << ':'
		<< conf_home << "/domains/" << split_dom(dom)
		<< '/' << dom << ":-::";
	
	string ln(domln.str());
	string prog(conf_home+"/bin/qmail_assign_add");
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
					return lr(err_no, "");
			case 1:
					return lr(err_exists, prog);
			}
	}
	return lr(err_exec,prog);
}

/**
 * Checks whether domain is in assign, if it's not returns 0, 
 * if it is returns err_dom_ex, on errors returns something else
 */
uint8_t cfsvq::assign_ex( const string &dom )
{
	string prog(conf_home+"/bin/qmail_assign_ex");
	char * const args[] = {
			const_cast<char *>(prog.c_str()),
			const_cast<char *>(dom.c_str()),
			NULL
	};

	int ret = run(args);
	if( WIFEXITED(ret) ) {
			switch(WEXITSTATUS(ret)) {
			case 1:
					return lr(err_no,"");
			case 0:
					return lr(err_exists,prog);
			}
	}
	return lr(err_exec, prog);
}
/**
 * Adds domain to assign file, it creates temporary file 
 * (name in tmpfs.assign_add) which you should rename to assign on success
 * \param dom domain
 * \return 0 on success
 */
uint8_t cfsvq::assign_rm( const string &dom )
{   
	ostringstream domln;
	domln << '+'<< dom << "-:" << ac_vq_user.val_str() << ':' 
		<< ac_vq_uid.val_str() << ':' << ac_vq_gid.val_str() << ':'
		<< conf_home << "/domains/" << split_dom(dom)
		<< '/' << dom << ":-::";
	
	string ln(domln.str());
	string prog(conf_home+"/bin/qmail_assign_rm");
	char * const args[] = {
			const_cast<char *>(prog.c_str()),
			const_cast<char *>(ln.c_str()),
			NULL
	};

	int ret = run(args);
	if( WIFEXITED(ret) ) {
			switch(WEXITSTATUS(ret)) {
			case 0: case 1: case 2:
					return lr(err_no, "");
			}
	}
	return lr(err_exec, prog);
}

/**
 * Adds host to rcpthosts, it creates temp. file 
 * (name in tmpfs.rcpt_add) which you should rename to rcpthosts on success,
 * if morercpthosts is changed rcpthosts is set to "", tmpfs.morercpt_add is set
 */
uint8_t cfsvq::rcpt_add(const string &dom)
{
	string prog(conf_home+"/bin/qmail_rcpthosts_add");
	char * const args[] = {
			const_cast<char *>(prog.c_str()),
			const_cast<char *>(dom.c_str()),
			NULL
	};

	int ret = run(args);
	if( WIFEXITED(ret) ) {
			switch(WEXITSTATUS(ret)) {
			case 0: case 1:
					return lr(err_no, "");
			case 2:
					return lr(err_over, prog);
			}
	}
	return lr(err_exec, prog);
}

/**
 * Remove host to rcpthosts, it creates temp. file 
 * (name in tmpfs.rcpt_rm) which you should rename to rcpthosts on success
 */
uint8_t cfsvq::rcpt_rm(const string &dom)
{
	char file[] = { qf_rcpthosts, '\0' };
	string prog(conf_home+"/bin/qmail_file_rm");
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
					return lr(err_no, "");
			}
	}
	return lr(err_exec,prog);
}

/**
 * Removes domain from locals, sets tmpfs.locals_rm to
 * name of created file, on success you have to rename it to locals
 */
uint8_t cfsvq::locals_rm(const string &dom)
{
	char file[] = { qf_locals, '\0' };
	string prog(conf_home+"/bin/qmail_file_rm");
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
					return lr(err_no, "");
			}
	}
	return lr(err_exec, prog);
}

/**
 * Remove line from a file. Lock original file, copy content to a temporary
 * file and return it's name. Caller will rename temporary file to original.
 * \param fn name of file
 * \param item line to remove
 * \param fntmp temporary file name
 * \return 0 if success
 */
uint8_t cfsvq::file_rm( const std::string & fn, const std::string & item,
		std::string & fntmp ) {

	fntmp = fn+uniq();
	string fnlck(fn+".lock");

	opfstream lock(fnlck.c_str());
	if( ! lock ) return lr(err_open, fnlck);

	if( lock_exnb(lock.rdbuf()->fd()) ) return lr(err_lckd, fnlck);

	ipfstream in(fn.c_str(),ios::in);
	if(!in) {
			if( errno == ENOENT ) return lr(err_no, "");
			return lr(err_open, fn);
	}
	opfstream tmp(fntmp.c_str(),ios::trunc);
	if(!tmp) return lr(err_open, fntmp);
	
	string ln;
	while( getline(in,ln) && tmp.good() ) {
			if(ln==item) continue;
			tmp<<ln<<endl;
	}
	if(in.bad()) {
			unlink(fntmp.c_str());
			return lr(err_rd, fn);
	}
	tmp.close();
	if(! tmp) {
			unlink(fntmp.c_str());
			return lr(err_wr, fntmp);
	}

	struct stat st;
	if(!fstat(in.rdbuf()->fd(), &st)) {
			if( chown(fntmp.c_str(), st.st_uid, st.st_gid) )
					return lr(err_chown, fntmp);
			if( chmod(fntmp.c_str(), st.st_mode & 07777 ) )
					return lr(err_chmod, fntmp);
	} else 
			return lr(err_stat, fntmp);
	
	return lr(err_no, "");
}

/**
 * Adds domain to virtualdomains, it creates temp. file (name in
 * tmpfs.virt_add) which should be renamed to virtualdomains on success
 */
uint8_t cfsvq::virt_add(const string &dom,const string &al)
{
	string ln(dom+':'+al);
	string prog(conf_home+"/bin/qmail_virtualdomains_add");
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
					return lr(err_no, "");
			case 1:
					return lr(err_exists, prog);
			}
	}
	return lr(err_exec, prog);
}

/**
 * Remove alias from virtualdomains. It does not check whether it's a domain
 * (if you want to remove only aliases you need to first use assign_ex to
 * check whether alias is in users/assign, if it is it means it's not an
 * alias).
 */
uint8_t cfsvq::virt_rm(const string &ali)
{
	string prog(conf_home+"/bin/qmail_virtualdomains_rm");
	char * const args[] = {
			const_cast<char *>(prog.c_str()),
			const_cast<char *>(ali.c_str()),
			NULL
	};

	int ret = run(args);
	if( WIFEXITED(ret) ) {
			switch(WEXITSTATUS(ret)) {
			case 0: case 1: case 2:
					return lr(err_no, "");
			}
	}
	return lr(err_exec, prog);
}

/**
 * Adds domain to morercpthosts, creates temp. file which should
 * be renamed to morercpthosts, you should also run morercpt_comp.
 */
uint8_t cfsvq::morercpt_add(const string &dom)
{
	string prog(conf_home+"/bin/qmail_mrh_add");
	char * const args[] = {
			const_cast<char *>(prog.c_str()),
			const_cast<char *>(dom.c_str()),
			NULL
	};

	int ret = run(args);
	if( WIFEXITED(ret) ) {
			switch(WEXITSTATUS(ret)) {
			case 0: case 1:
					return lr(err_no, "");
			}
	}
	return lr(err_exec, prog);
}

/**
 * Add line to a file. Create temporary file, copy content to it,
 * add item, return temporary file's name.
 * \param fn file when we add a line
 * \param item line to add
 * \param fntmp temp. file's name
 * \param beg_at if true add line at begining
 * \return 0 if success
 */
uint8_t cfsvq::file_add( const string & fn, const string &item,
		string & fntmp, bool beg_at ) {

	fntmp = fn+uniq();
	string fnlck(fn+".lock");
	opfstream lock(fnlck.c_str());
	if( ! lock ) return lr(err_open, fnlck);

	if( lock_exnb(lock.rdbuf()->fd()) ) return lr(err_lckd, fnlck);

	ipfstream in(fn.c_str());
	bool enoent = false;
	if(!in) {
			if( errno != ENOENT ) return lr(err_open, fn);
			enoent = true;
	}

	opfstream tmp(fntmp.c_str(),ios::trunc);
	if(!tmp) return lr(err_open, fntmp);

	if( ! enoent ) {
			string ln;
			if(beg_at) tmp<<item<<endl;

			while( getline(in,ln) && tmp.good() ) {
					if( item == ln ) continue;
					tmp<<ln<<endl;
			}
			if( in.bad() ) {
					unlink(fntmp.c_str());
					return lr(err_rd, fn);
			}
	}
	if(!beg_at) tmp<<item<<endl;
	tmp.close();
	if( tmp.bad() ) {
			unlink(fntmp.c_str());
			return lr(err_wr, fntmp);
	}

	if( ! enoent ) {
			// try to preserve all of file's information, ignore errors
			struct stat st;
			if(!fstat(in.rdbuf()->fd(), &st)) {
					if( chown( fntmp.c_str(), st.st_uid, st.st_gid) )
							return lr(err_chown, fntmp);
					if( chmod( fntmp.c_str(), st.st_mode & 07777 ) )
							return lr(err_chmod, fntmp);
			} else
					return lr(err_stat, fn);
	} else if( chmod(fntmp.c_str(), ac_vq_fmode.val_int()) )
			return lr(err_chmod, fntmp);
	
	return lr(err_no, "");
}

/**
 * Adds domain to morercpthosts, creates temp. file which should
 * be renamed to morercpthosts, you should also run morercpt_comp.
 */
uint8_t cfsvq::morercpt_rm(const string &dom)
{
	string prog(conf_home+"/bin/qmail_mrh_rm");
	char * const args[] = {
			const_cast<char *>(prog.c_str()),
			const_cast<char *>(dom.c_str()),
			NULL
	};

	int ret = run(args);
	if( WIFEXITED(ret) ) {
			switch(WEXITSTATUS(ret)) {
			case 0: case 1: case 2:
					return lr(err_no, "");
			}
	}
	return lr(err_exec, prog);
}

/**
 * Create maildir 
 * \param d directory for maildir (new,cur,tmp will be created as subdirectories of it)
 * \return err_no on success
 */
uint8_t cfsvq::maildirmake(const string & d)
{
	if( mkdir(d.c_str(), ac_vq_mode.val_int() )
		|| chown(d.c_str(), ac_vq_uid.val_int(), ac_vq_gid.val_int())) return 1;
	string dir = d+"/new"; 
	if( mkdir(dir.c_str(), ac_vq_mode.val_int() ) 
		|| chown(dir.c_str(), ac_vq_uid.val_int(), ac_vq_gid.val_int())) return 1;
	dir = d+"/cur";
	if( mkdir(dir.c_str(), ac_vq_mode.val_int() ) 
		|| chown(dir.c_str(), ac_vq_uid.val_int(), ac_vq_gid.val_int())) return 1;
	dir = d+"/tmp";
	if( mkdir(dir.c_str(), ac_vq_mode.val_int() ) 
		|| chown(dir.c_str(), ac_vq_uid.val_int(), ac_vq_gid.val_int())) return 1;
	return 0;
}

/**
 * Create path to quota file
 * \return path to quota file of specified user\@domain
 */
string cfsvq::qtfile(const string &d, const string &u) {
	string dom(lower(d)), user(lower(u));
	string qtfile = conf_home+"/domains/"+split_dom(dom)+'/'+dom
			+'/'+split_user(user)+'/'+user+"/Maildir/.quota";
	return qtfile;
}

/**
 * Create path to quota file
 * \return path to quota file of specified user\@domain
 */
string cfsvq::qtfile(const string &d) {
	string dom(lower(d));
	string qtfile = conf_home+"/domains/"+split_dom(dom)+'/'+dom
			+"/.quota";
	return qtfile;
}
/**
 * \return path to default quota file of specified domain
 */
string cfsvq::qtfile_def(const string &d) {
	string dom(lower(d));
	string qtfile = conf_home+"/domains/"+split_dom(dom)+'/'+dom
			+"/.quota-default";
	return qtfile;
}
/**
 * \return path to quota file of specified domain
 */
string cfsvq::qtfile_def() {
	return conf_home+"/domains/.quota-default";
}

/**
 * Set current usage of user's mailbox.
 * \param qtfile quota file
 * \param c value added to current quota (can be <0)
 */
uint8_t cfsvq::qtf_cur_set(const string &qtfile, int32_t c) {
	quota_value cq;
	pfstream qtf(qtfile.c_str(), ios::in | ios::out);
	if(!qtf) return lr(err_open, qtfile);

	qtf.read(reinterpret_cast<char *>(&cq), sizeof(cq));
	if ( c < 0 ) {
	  if( -c > cq ) cq = 0;
	  else cq+=c;
	} else if( c > 0 ) {
	  if ( cq >= 0xffffffff/2 && cq<=c ) cq = 0xffffffff;
	  else cq += c;
	} else return lr(err_no, "");

	qtf.seekp(0);
	qtf.write(reinterpret_cast<char *>(&cq),sizeof(cq));
	qtf.flush();
	qtf.close();
	
	if(!qtf) return lr(err_wr, qtfile);
	return lr(err_no, "");
}

/**
 * Set current usage of mailbox.
 * \param d domain
 * \param u user
 * \param c value added to current usage
 */
uint8_t cfsvq::qt_cur_set(const string&d, const string &u, int32_t c) {
	return qtf_cur_set(qtfile(d,u), c);
}

/**
 * Set current usage for domain
 * \param d domain
 * \param c value added to current usage
 */
uint8_t cfsvq::qt_dom_cur_set(const string &d, int32_t c) {
	return qtf_cur_set(qtfile(d), c);
}

/**
 * \param qtfile quota file
 * \param c max quota
 */
uint8_t cfsvq::qtf_set(const string &qtfile, quota_value c) {
	pfstream qtf(qtfile.c_str(), ios::in | ios::out );
	quota_value z = 0;
	if(qtf) {
			qtf.read(reinterpret_cast<char *>(&z),sizeof(z));
			qtf.clear();
			qtf.seekp(0, ios::beg);
	} else {
			qtf.clear();
			qtf.open(qtfile.c_str(), ios::out );
			if(!qtf) return lr(err_open, qtfile);
			
			if(fchown(qtf.rdbuf()->fd(), ac_vq_uid.val_int(), ac_vq_gid.val_int() ) )
					return lr(err_chown, qtfile);
			if(fchmod(qtf.rdbuf()->fd(), ac_vq_fmode.val_int()) )
					return lr(err_chmod, qtfile);
	}			
	qtf.write(reinterpret_cast<char *>(&z),sizeof(z));
	qtf.write(reinterpret_cast<char *>(&c),sizeof(c));
	qtf.flush();
	qtf.close();
	if(!qtf) return lr(err_wr, qtfile);
	return lr(err_no, "");
}

/**
 * Set quota for user
 * \param d domain
 * \param u user
 * \param c max quota
 * \return err_no on success
 */
uint8_t cfsvq::qt_set(const string &d, const string &u, quota_value c) {
	return qtf_set(qtfile(d,u), c);
}

/**
 * Set quota for domain
 * \param d domain
 * \param c max quota
 * \return err_no on success
 */
uint8_t cfsvq::qt_dom_set(const string &d, quota_value c) {
	return qtf_set(qtfile(d), c);
}

/**
 * \param qtfile quota file
 * \param cm quota max
 * \param cc value of current use
 */
uint8_t cfsvq::qtf_set(const string &qtfile, quota_value cm, quota_value cc) {
	opfstream qtf(qtfile.c_str());
	if(!qtf) return lr(err_open, qtfile);
	qtf.write(reinterpret_cast<char *>(&cc),sizeof(cc));
	qtf.write(reinterpret_cast<char *>(&cm),sizeof(cm));
	qtf.flush();
	qtf.close();
	if(!qtf) return lr(err_wr, qtfile);
	return lr(err_no, "");
}

/**
 * Set max and current quota.
 * \param d domain
 * \param u user
 * \param qm quota max
 * \param qc current usage
 */
uint8_t cfsvq::qt_set(const string &d, const string &u, quota_value qm, quota_value qc) {
	return qtf_set(qtfile(d,u), qm, qc);
}

/**
 * Set max and current quota for domain
 * \param d domain
 * \param qm max value
 * \param qc current value
 */
uint8_t cfsvq::qt_dom_set(const string &d, quota_value qm, quota_value qc) {
	return qtf_set(qtfile(d), qm, qc);
}

/**
 * Get quote value from specified file
 * \param qtfile quota file
 * \param cm changed to quota max
 * \param cc changed to current use
 */
uint8_t cfsvq::qtf_get(const string &qtfile, quota_value & cm, quota_value & cc) {
	ipfstream qtf(qtfile.c_str());
	if(!qtf) return lr(err_open, qtfile);
	qtf.read(reinterpret_cast<char *>(&cc),sizeof(cc));
	qtf.read(reinterpret_cast<char *>(&cm),sizeof(cm));
	qtf.close();
	if(!qtf) return lr(err_wr, qtfile);
	return lr(err_no, "");
}

/**
 * Get quota for user
 * \param d domain
 * \param u user
 * \param qm max value
 * \param qc current value
 */
uint8_t cfsvq::qt_get(const string &d, const string &u, 
		quota_value &qm, quota_value & qc) {
	return qtf_get(qtfile(d,u).c_str(), qm, qc);
}

/**
 * Get quota for domain
 * \param d domain
 * \param qm max value
 * \param qc current value
 */
uint8_t cfsvq::qt_dom_get(const string&d, quota_value & qm, quota_value & qc) {
	return qtf_get(qtfile(d).c_str(), qm, qc);
}

/**
 * \brief check wheter user used his quota
 * \return 0 - means no, ... - yes, user over quota
*/
uint8_t cfsvq::qtf_over(const string &qtfile) {
	ipfstream qtf(qtfile.c_str());
	if(!qtf) return 0; // no quota file
	quota_value qc, qm;
	qtf.read(reinterpret_cast<char *>(&qc),sizeof(qc));
	qtf.read(reinterpret_cast<char *>(&qm),sizeof(qm));
	if(!qtf || !qm) return 0;
	return qc < qm ? 0 : 1;
}

/**
 * Checks whether user is over quota
 * \see cfsvq::qtf_over
 */
uint8_t cfsvq::qt_over(const string &d, const string &u) {
	return qtf_over(qtfile(d,u));
}

/**
 * Checks whether domain is over quota
 * \see cfsvq::qtf_over
 */
uint8_t cfsvq::qt_dom_over(const string &d) {
	return qtf_over(qtfile(d));
}

/**
 * set default user quota for this domain
 */
uint8_t cfsvq::qt_def_set(const string &d, quota_value qm) {
	return qtf_set(qtfile_def(d), qm);
}

/**
 * set default quota for domains
 */
uint8_t cfsvq::qt_dom_def_set(quota_value qm) {
	return qtf_set(qtfile_def(), qm);
}

/**
 * get default user quota for this domain
 */
uint8_t cfsvq::qt_def_get(const string &d, quota_value & qm) {
	quota_value qc;
	return qtf_get(qtfile_def(d), qm, qc);
}

/**
 * get default quota for domains
 */
uint8_t cfsvq::qt_dom_def_get(quota_value & qm) {
	quota_value qc;
	return qtf_get(qtfile_def(), qm, qc);
}

/**
 * set user's quota from default domain's quota
 */
uint8_t cfsvq::qt_def_cp(const string &d, const string &u) {
	uint8_t ret;
	quota_value qc, qm;
	ret = qtf_get(qtfile_def(d), qm, qc);
	if( ret ) return lr(ret, "");
	return lr(qt_set(d,u,qm), "");
}

/**
 * set domain's quota from default quota
 */
uint8_t cfsvq::qt_dom_def_cp(const string &d) {
	uint8_t ret;
	quota_value qc, qm;
	ret = qtf_get(qtfile_def(), qm, qc);
	if( ret ) return lr(ret, "");
	return lr(qt_dom_set(d,qm), "");
}

/**
 * Checks whether auth is initalized, if not try to do that
 */
void cfsvq::assert_auth() {
	if(!auth.get())
			auth.reset(cauth::alloc());
}

/**
 * Adds mailbox configuration
 * \param d domain
 * \param u user
 * \param e address's extension
 * \param ui configuration
 */
uint8_t cfsvq::udot_add(const string &d, const string &u, const string &e,
		udot_info & ui ) {
	assert_auth();
	string dom(lower(d)), user(lower(u)), ext(lower(e));
	string df(dotfile(dom, user, ext)), dftmp;
	string ln(udot_ln(ui));
	if(ln.empty()) return lr(err_no, "");

	if( file_add(df, ln, dftmp, ui.type == autoresp || ui.type == sms) )
			return lastret;
	
	if(auth->udot_add(dom,user,ext,ui)) {
			unlink(dftmp.c_str());
			return lr(err_auth, auth->err_report());
	}
	if( rename(dftmp.c_str(), df.c_str()) ) {
			unlink(dftmp.c_str());
			return lr(err_ren, dftmp);
	}
	return lr( err_no, "" );
}

/** 
 * Remove entry with given id.
 * \param d domain
 * \param user user
 * \param ext address's extention
 * \param id entry's id.
 */
uint8_t cfsvq::udot_rm(const string &d, 
				const string &user, const string &ext,
				const string & id) {
	assert_auth();
	udot_info ui;
	ui.id = id;
	if(udot_get(d, ui)) return lastret;

	string dom(lower(d));
	string df(dotfile(dom, lower(user), lower(ext)));
	string ln(udot_ln(ui));
	if(ln.empty()) return lr(err_no, "");

	// is it portable?? may be on some systems it will block?
	opfstream dout;
	string dftmp;
	if( tmp_crt(df, dout, dftmp) != err_no ) return lastret;
	
	ipfstream din(df.c_str());
	if(din) {
			string cln;
			while(getline(din, cln)) {
					if(cln!=ln && !cln.empty()) {
							dout<<cln<<endl;
							if(!dout) return lr(err_wr, dftmp);
					}
			}
			if(din.bad()) return lr(err_rd, df);
			din.clear();
	}

	dout.flush();
	if(!dout) return lr(err_wr, dftmp);

	if(auth->udot_rm(dom,id)) return lr(err_auth, auth->err_report());
	return lr(rename(dftmp.c_str(), df.c_str()) ? err_ren : err_no, dftmp);
}

/**
 * Lists all mailbox configurations for e-mail
 * \param d domain
 * \param u user
 * \param e extension
 * \param uideq information read from database
 */
uint8_t cfsvq::udot_ls(const string &d, const string &u,
		const string &e, vector<udot_info> & uideq) {
	assert_auth();
	string dom(lower(d)), user(lower(u));
	if(auth->udot_ls(dom,user,lower(e),uideq)) return lr(err_auth, auth->err_report());
	return lr(err_no, "");
}

/**
 * Lists all entries of specified type
 * \param d domain
 * \param u user
 * \param e extension
 * \param uideq information read from database
 * \param type type of entries
 */
uint8_t cfsvq::udot_ls(const string &d, const string &u,
		const string &e, udot_type type, vector<udot_info> & uideq) {
	assert_auth();
	string dom(lower(d)), user(lower(u));
	if(auth->udot_ls(dom,user,lower(e),type,uideq)) return lr(err_auth, auth->err_report());
	return lr(err_no, "");
}

/**
 * Changes entry
 */
uint8_t cfsvq::udot_rep(const string &d, const string &u, 
		const string &e, const udot_info & uin) {
	assert_auth();

	udot_info uio;
	uio.id = uin.id;
	if(udot_get(d, uio)) return lastret;

	string dom(lower(d));
	string df(dotfile(dom, lower(u), lower(e)));
	string lno(udot_ln(uio)), lnn(udot_ln(uin));
	if(lno.empty()) return lr(err_no, "");

	// is it portable?? may be on some systems it will block?
	opfstream dout;
	string dftmp;
	if( tmp_crt(df, dout, dftmp) != err_no ) return(lastret); 
	
	ipfstream din(df.c_str());
	if(din) {
			string cln;

			if( uin.type == autoresp || uin.type == sms )
					dout<<lnn<<endl;

			while(getline(din, cln)) {
					if(cln!=lno && !cln.empty()) {
							dout<<cln<<endl;
							if(!dout) return lr(err_wr, dftmp);
					}
			}
		
			if( ! ( uin.type == autoresp || uin.type == sms ) )
					dout<<lnn<<endl;

			if(din.bad()) return lr(err_rd, df);
			din.clear();
	} else 
			dout<<lnn<<endl;
	dout.flush();
	if(!dout) return lr(err_wr, dftmp);

	if(auth->udot_rep(dom,uin)) return lr(err_auth, auth->err_report());
	return lr(rename(dftmp.c_str(), df.c_str()) ? err_ren : err_no, dftmp);
}

/**
 * Get all configuration for domain
 */
uint8_t cfsvq::udot_get(const string &dom, udot_info &ui) {
	assert_auth();
	return lr(auth->udot_get(lower(dom), ui) ? err_auth : err_no, auth->err_report());
}

/**
 * Create path for dot-qmail file
 * \param d domain
 * \param u user
 * \param e extension
 */
string cfsvq::dotfile(const string &d, const string &u, const string &e) {
	string dom(d), user(u), ext(e);
	string dotfile;
	dotfile.reserve(2000);
	dotfile.append(conf_home);
	dotfile.append("/domains/");
	dotfile.append(split_dom(dom));
	dotfile.append("/", 1);
	dotfile.append(dom);
	dotfile.append("/", 1);
	dotfile.append(split_user(user));
	dotfile.append("/.qmail-");
	replace(user.begin(),user.end(), '.', ':');
	dotfile.append(user);
	if(!ext.empty()) {
			replace(ext.begin(),ext.end(), '.', ':');
			dotfile += '-'+ext;
	}
	return dotfile;
}

/**
 * \return 0 if type is not supported
 */
uint8_t cfsvq::udot_sup_is(udot_type t) {
	unsigned ud_supl = sizeof(ud_supl);
	if(!ud_supl) return 0;
	ud_supl--;
	do {
			if(ud_sup[ud_supl] == t) return 1;
	} while(ud_supl--);
	return 0;
}

/**
 * Create line which will be add do dot-qmail
 */
string cfsvq::udot_ln(const udot_info &ui) {
	string ret;
	string::size_type pos, pos1;
	string tmp, tmp1;
	switch(ui.type) {
	case redir:
			ret.reserve(ui.val.length()+3);
			ret = '&';
			ret += ui.val;
			break;
	case autoresp:
			ret = "|";
			ret += conf_home+"/bin/autoresp ";
			tmp1 = hex_from(ui.val);

			pos = tmp1.find('\0');
			if( pos == string::npos )
					throw logic_error("udot_ln: there's no message!", __FILE__, __LINE__);
			
			tmp = tmp1.substr(0, pos);
			ret += tmp;
			
			pos1 = tmp1.find('\0', ++pos);
			tmp = tmp1.substr(pos, pos1 - pos);
			ret += ' ';
			ret += to_hex((uint8_t *) tmp.data(), tmp.length());
			
			if( pos1 != string::npos 
				&& (pos1 = tmp1.find('\0', (pos=pos1+1))) != string::npos ) {
					tmp = tmp1.substr(pos, pos1-pos);
					if( ! tmp.empty() ) {
							ret+=' ';
							ret+=to_hex((uint8_t *) tmp.data(), tmp.length());
					}
			}
			break;
	default:
			ret = ui.val;
	}
	return ret;
}

/**
 * Is mailbox has entry with this type
 * \param dom domain
 * \param user user
 * \param ext extension
 * \param type type to find
 */
uint8_t cfsvq::udot_has(const string &dom, const string &user, 
		const string &ext, udot_type type) {
	assert_auth();
	return auth->udot_has(dom, user, ext, type);
}

/**
 * Add to dot-qmail default entries
 */
uint8_t cfsvq::udot_add_md_def(const string &d, const string &u,
		const string &e) {
	udot_info ui;
	ui.type = maildir;
	ui.val = "./"+lower(u)+"/Maildir/";
	return udot_add(d, u, e, ui);
}

/**
 * creates temporary file, chown(vq), chmod(vq_mode)
 */
uint8_t cfsvq::tmp_crt(const string &fn, opfstream & out, string &fnout) {
	ostringstream pid;
	pid<<getpid();
	
	fnout = fn + '.' + pid.str() + tmp_end;
	if(out.is_open()) out.close();
	out.open(fnout.c_str(), ios::trunc);
	if(!out) return lr(err_open, fnout);

	if( fchown(out.rdbuf()->fd(), ac_vq_uid.val_int(), ac_vq_gid.val_int()) )
			return lr(err_chown, fnout);
	if( fchmod(out.rdbuf()->fd(), ac_vq_fmode.val_int() ) )
			return lr(err_chmod, fnout);
	return lr(err_no, "");
}

/**
 * Remove all entries of given type
 * \param d domain
 * \param u user
 * \param e extension
 * \param type delete this type
 */
uint8_t cfsvq::udot_rm(const string &d, const string &u,
		const string &e, udot_type type) {
	assert_auth();
	vector<udot_info> uis;
	vector<udot_info>::iterator uis_end, uis_cur;
	if(udot_ls(d, u, e, type, uis)) return(lastret);
	uis_end = uis.end();
	
	string dom(lower(d));
	string df(dotfile(dom, lower(u), lower(e)));

	for( uis_cur=uis.begin(); uis_cur != uis_end; ++ uis_cur ) {
			uis_cur->val = udot_ln(*uis_cur);
	}

	// is it portable?? may be on some systems it will block?
	opfstream dout;
	string dftmp;
	if( tmp_crt(df, dout, dftmp) != err_no ) return(lastret);
	
	ipfstream din(df.c_str());
	if(din) {
			string cln;
			while(getline(din, cln)) {
					if( cln.empty() ) continue;
					for( uis_cur = uis.begin(); uis_cur != uis_end; uis_cur ++ ) {
							if( cln == uis_cur->val ) break;
					}
					if( uis_cur == uis_end ) {
							dout<<cln<<endl;
							if(!dout) return lr(err_wr, dftmp);
					}
			}
			if(din.bad()) return lr(err_rd, df);
			din.clear();
	}

	dout.flush();
	if(!dout) return lr(err_wr, dftmp);
	if(auth->udot_rm(dom,u,e,type)) return lr(err_auth, auth->err_report());
	return lr(rename(dftmp.c_str(), df.c_str()) ? err_ren : err_no, dftmp);
}

/**
 * Count entries of this type
 * \param d domain
 * \param u user
 * \param e extension
 * \param type
 * \param cnt number of entries found
 */
uint8_t cfsvq::udot_type_cnt( const string &d, const string &u, const string &e,
		udot_type type, size_type &cnt) {
	assert_auth();
	return lr( auth->udot_type_cnt(d,u,e,type,cnt) ? err_auth : err_no, auth->err_report() );
}

