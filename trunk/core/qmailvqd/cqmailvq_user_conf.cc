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
#include "cqmailvq_common.hpp"

#include <pfstream.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <sstream>

namespace POA_vq {
	using namespace std;
	using namespace posix;

	/**
	 * Adds mailbox configuration
	 * \param d domain
	 * \param u user
	 * \param e address's extension
	 * \param ui configuration
	 */
	cqmailvq::error * cqmailvq::user_conf_add(const char * dom_id, 
			const char *login, const char *pfix, user_conf_info & ui ) {
		string dom(text::lower(d)), user(text::lower(u)), ext(text::lower(e));
		string df(dotfile(dom, user, ext)), dftmp;
		string ln(user_conf_ln(ui));
		if(ln.empty()) return lr(::vq::ivq::err_no, "");
	
		if( file_add(df, ln, dftmp, ui.type == autoresp || ui.type == sms) )
				return lastret;
		
		if(auth->user_conf_add(dom,user,ext,ui)) {
				unlink(dftmp.c_str());
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		}
		if( rename(dftmp.c_str(), df.c_str()) ) {
				unlink(dftmp.c_str());
				return lr(::vq::ivq::err_ren, dftmp);
		}
		return lr( ::vq::ivq::err_no, "" );
	} std_catch

#if 0
	/** 
	 * Remove entry with given id.
	 * \param d domain
	 * \param user user
	 * \param ext address's extention
	 * \param id entry's id.
	 */
	cqmailvq::error * cqmailvq::user_conf_rm(const string &d, 
					const string &user, const string &ext,
					const string & id) {
		string dom(text::lower(d));
		user_conf_info ui;
		ui.id = id;
		if(user_conf_get(dom, ui)) return lastret;
	
		string ln(user_conf_ln(ui));
		string df(dotfile(dom, text::lower(user), text::lower(ext)));
		if(ln.empty()) return lr(::vq::ivq::err_no, "");
	
		// is it portable?? may be on some systems it will block?
		opfstream dout;
		string dftmp;
		if( tmp_crt(df, dout, dftmp) != ::vq::ivq::err_no ) return lastret;
		
		ipfstream din(df.c_str());
		if(din) {
				string cln;
				while(getline(din, cln)) {
						if(cln!=ln && !cln.empty()) {
								dout<<cln<<endl;
								if(!dout) return lr(::vq::ivq::err_wr, dftmp);
						}
				}
				if(din.bad()) return lr(::vq::ivq::err_rd, df);
				din.clear();
		}
	
		dout.flush();
		if(!dout) return lr(::vq::ivq::err_wr, dftmp);
	
		if(auth->user_conf_rm(dom,id)) return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(rename(dftmp.c_str(), df.c_str()) ? ::vq::ivq::err_ren : ::vq::ivq::err_no, dftmp);
	} std_catch
	
	/**
	 * Lists all mailbox configurations for e-mail
	 * \param d domain
	 * \param u user
	 * \param e extension
	 * \param uideq information read from database
	 */
	cqmailvq::error * cqmailvq::user_conf_ls(const string &d, const string &u,
			const string &e, vector<user_conf_info> & uideq) {
		string dom(text::lower(d)), user(text::lower(u));
		if(auth->user_conf_ls(dom,user,text::lower(e),uideq)) return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Lists all entries of specified type
	 * \param d domain
	 * \param u user
	 * \param e extension
	 * \param uideq information read from database
	 * \param type type of entries
	 */
	cqmailvq::error * cqmailvq::user_conf_ls(const string &d, const string &u,
			const string &e, user_conf_type type, vector<user_conf_info> & uideq) {
		if(auth->user_conf_ls(text::lower(d),text::lower(u),text::lower(e),type,uideq)) 
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Changes entry
	 */
	cqmailvq::error * cqmailvq::user_conf_rep(const string &d, const string &u, 
			const string &e, const user_conf_info & uin) {
		string dom(text::lower(d));
		user_conf_info uio;
		uio.id = uin.id;
		if(user_conf_get(dom, uio)) return lastret;
	
		string df(dotfile(dom, text::lower(u), text::lower(e)));
		string lno(user_conf_ln(uio)), lnn(user_conf_ln(uin));
		if(lno.empty()) return lr(::vq::ivq::err_no, "");
	
		// is it portable?? may be on some systems it will block?
		opfstream dout;
		string dftmp;
		if( tmp_crt(df, dout, dftmp) != ::vq::ivq::err_no ) return(lastret); 
		
		ipfstream din(df.c_str());
		if(din) {
				string cln;
	
				if( uin.type == autoresp || uin.type == sms )
						dout<<lnn<<endl;
	
				while(getline(din, cln)) {
						if(cln!=lno && !cln.empty()) {
								dout<<cln<<endl;
								if(!dout) return lr(::vq::ivq::err_wr, dftmp);
						}
				}
			
				if( ! ( uin.type == autoresp || uin.type == sms ) )
						dout<<lnn<<endl;
	
				if(din.bad()) return lr(::vq::ivq::err_rd, df);
				din.clear();
		} else 
				dout<<lnn<<endl;
		dout.flush();
		if(!dout) return lr(::vq::ivq::err_wr, dftmp);
	
		if(auth->user_conf_rep(dom,uin)) return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(rename(dftmp.c_str(), df.c_str()) ? ::vq::ivq::err_ren : ::vq::ivq::err_no, dftmp);
	} std_catch
	
	/**
	 * Get all configuration for domain
	 */
	cqmailvq::error * cqmailvq::user_conf_get(const string &dom, user_conf_info &ui) std_try {
		if(auth->user_conf_get(text::lower(dom), ui))
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
#endif // if 0	
	/**
	 * Create path for dot-qmail file
	 * \param d domain
	 * \param u user
	 * \param e extension
	 */
	string cqmailvq::dotfile(const string &dom, const string &u, const string &e) std_try {
		string user(u), ext(e);
		string dotfile(user_root_path(dom, u));
		dotfile.append("/.qmail-");
		replace(user.begin(),user.end(), '.', ':');
		dotfile.append(user);
		if(!ext.empty()) {
				replace(ext.begin(),ext.end(), '.', ':');
				dotfile += '-'+ext;
		}
		return dotfile;
	} std_catch
#if 0
	/**
	 * \return 0 if type is not supported
	 */
	cqmailvq::error * cqmailvq::user_conf_sup_is(user_conf_type t) std_try {
		unsigned ud_supl = sizeof(ud_supl);
		if(!ud_supl) return 0;
		ud_supl--;
		do {
				if(ud_sup[ud_supl] == t) return 1;
		} while(ud_supl--);
		return 0;
	} std_catch
	
	/**
	 * Create line which will be add do dot-qmail
	 */
	string cqmailvq::user_conf_ln(const user_conf_info &ui) std_try {
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
				ret += home+"/bin/autoresp ";
				tmp1 = hex_from(ui.val);
	
				pos = tmp1.find('\0');
				if( pos == string::npos )
						throw logic_error("user_conf_ln: there's no message!", __FILE__, __LINE__);
				
				tmp = tmp1.substr(0, pos);
				ret += tmp;
				
				pos1 = tmp1.find('\0', ++pos);
				tmp = tmp1.substr(pos, pos1 - pos);
				ret += ' ';
				ret += to_hex((cqmailvq::error * *) tmp.data(), tmp.length());
				
				if( pos1 != string::npos 
					&& (pos1 = tmp1.find('\0', (pos=pos1+1))) != string::npos ) {
						tmp = tmp1.substr(pos, pos1-pos);
						if( ! tmp.empty() ) {
								ret+=' ';
								ret+=to_hex((cqmailvq::error * *) tmp.data(), tmp.length());
						}
				}
				break;
		default:
				ret = ui.val;
		}
		return ret;
	} std_catch
	
	/**
	 * Is mailbox has entry with this type
	 * \param dom domain
	 * \param user user
	 * \param ext extension
	 * \param type type to find
	 */
	cqmailvq::error * cqmailvq::user_conf_has(const string &dom, const string &user, 
			const string &ext, user_conf_type type) {
		if(auth->user_conf_has(text::lower(dom), text::lower(user), text::lower(ext), type))
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Add to dot-qmail default entries
	 */
	cqmailvq::error * cqmailvq::user_conf_add_md_def(const string &d, const string &u,
			const string &e) {
		user_conf_info ui;
		ui.type = maildir;
		ui.val = "./"+text::lower(u)+"/Maildir/";
		return user_conf_add(d, u, e, ui);
	} std_catch
#endif // if 0	
	/**
	 * creates temporary file, chown(vq), chmod(vq_mode)
	 */
	cqmailvq::error * cqmailvq::tmp_crt(const string &fn, opfstream & out, string &fnout) std_try {
		ostringstream pid;
		pid<<getpid();
		
		fnout = fn + '.' + pid.str() + tmp_end;
		if(out.is_open()) out.close();
		out.open(fnout.c_str(), ios::trunc);
		if(!out) return lr(::vq::ivq::err_open, fnout);
	
		if( fchown(out.rdbuf()->fd(), this->uid, this->gid) )
				return lr(::vq::ivq::err_chown, fnout);
		if( fchmod(out.rdbuf()->fd(), this->fmode ) )
				return lr(::vq::ivq::err_chmod, fnout);
		return lr(::vq::ivq::err_no, "");
	} std_catch
#if 0
	/**
	 * Remove all entries of given type
	 * \param d domain
	 * \param u user
	 * \param e extension
	 * \param type delete this type
	 */
	cqmailvq::error * cqmailvq::user_conf_rm(const string &d, const string &u,
			const string &e, user_conf_type type) {
		vector<user_conf_info> uis;
		vector<user_conf_info>::iterator uis_end, uis_cur;
		if(user_conf_ls(d, u, e, type, uis)) return(lastret);
		uis_end = uis.end();
		
		string dom(text::lower(d));
		string df(dotfile(dom, text::lower(u), text::lower(e)));
	
		for( uis_cur=uis.begin(); uis_cur != uis_end; ++ uis_cur ) {
				uis_cur->val = user_conf_ln(*uis_cur);
		}
	
		// is it portable?? may be on some systems it will block?
		opfstream dout;
		string dftmp;
		if( tmp_crt(df, dout, dftmp) != ::vq::ivq::err_no ) return(lastret);
		
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
								if(!dout) return lr(::vq::ivq::err_wr, dftmp);
						}
				}
				if(din.bad()) return lr(::vq::ivq::err_rd, df);
				din.clear();
		}
	
		dout.flush();
		if(!dout) return lr(::vq::ivq::err_wr, dftmp);
		if(auth->user_conf_rm(dom,u,e,type)) return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report());
		return lr(rename(dftmp.c_str(), df.c_str()) ? ::vq::ivq::err_ren : ::vq::ivq::err_no, dftmp);
	} std_catch
	
	/**
	 * Count entries of this type
	 * \param d domain
	 * \param u user
	 * \param e extension
	 * \param type
	 * \param cnt number of entries found
	 */
	cqmailvq::error * cqmailvq::user_conf_type_cnt( const string &d, const string &u, const string &e,
			user_conf_type type, size_type &cnt) {
		if( auth->user_conf_type_cnt(d,u,e,type,cnt) )
				return lr(::vq::ivq::err_auth, auth->::vq::ivq::err_report() );
		return lr(::vq::ivq::err_no, "");
	} std_catch
#endif  // if 0
} // namespace POA_vq
