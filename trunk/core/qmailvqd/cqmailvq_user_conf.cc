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
#include <text.hpp>

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
			const char *_login, const char *_pfix, user_conf_info & ui ) std_try {

		if( !dom_id || !_login || !_pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		string login(text::lower(_login)), pfix(text::lower(_pfix));

		string ln;
		auto_ptr<error> ret(user_conf_ln(ui, ln));
		if( ::vq::ivq::err_func_ni == ret->ec ) {
				return auth->user_conf_add(dom_id, login.c_str(), 
					pfix.c_str(), ui);
		}
		
		string df(dotfile(dom_id, login, pfix)), dftmp;

		ret.reset(file_add(df, ln, dftmp, ui.type == ::vq::ivq::uc_autoresp ));
		if( ::vq::ivq::err_no != ret->ec )
				return ret.release();
		
		ret.reset(auth->user_conf_add(dom_id, login.c_str(), pfix.c_str(), ui));
		if( ::vq::ivq::err_no != ret->ec ) {
				unlink(dftmp.c_str());
				return ret.release();
		}
		if( rename(dftmp.c_str(), df.c_str()) ) {
				unlink(dftmp.c_str());
				return lr(::vq::ivq::err_ren, dftmp);
		}
		return lr( ::vq::ivq::err_no, "" );
	} std_catch

	/** 
	 * Remove entry with given id.
	 * \param d domain
	 * \param user user
	 * \param ext address's extention
	 * \param id entry's id.
	 */
	cqmailvq::error * cqmailvq::user_conf_rm(const char * dom_id, 
			const char *_login, const char *_pfix,
			const char * id) std_try {

		if( ! dom_id || ! _login || ! _pfix || ! id )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		user_conf_info ui;
		ui.id_conf = id;
		auto_ptr<error> ret(user_conf_get(ui)); 
		if( ::vq::ivq::err_no != ret->ec )
				return ret.release();
	
		string ln;
		ret.reset(user_conf_ln(ui, ln));
		if( ::vq::ivq::err_func_ni == ret->ec ) {
				return auth->user_conf_rm(ui.id_conf);
		}
		
		string df(dotfile(dom_id, text::lower(_login), text::lower(_pfix)));
	
		// is it portable?? may be on some systems it will block?
		opfstream dout;
		string dftmp;
		ret.reset( tmp_crt(df, dout, dftmp) );
		if( ::vq::ivq::err_no != ret->ec ) 
				return ret.release();
		
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
	
		ret.reset(auth->user_conf_rm(ui.id_conf));
		if( ::vq::ivq::err_no != ret->ec )
				return ret.release();
		if(rename(dftmp.c_str(), df.c_str()))
				return lr( ::vq::ivq::err_ren, dftmp);
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 * Lists all mailbox configurations for e-mail
	 * \param d domain
	 * \param u user
	 * \param e extension
	 * \param uideq information read from database
	 */
	cqmailvq::error * cqmailvq::user_conf_ls(const char * dom_id, 
			const char * _login, const char * _pfix,
			user_conf_info_list_out ucis ) std_try {
		
		if( ! dom_id || ! _login || ! _pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
		string login(text::lower(_login));
		string pfix(text::lower(_pfix));
		return auth->user_conf_ls( dom_id, login.c_str(), pfix.c_str(), ucis);
	} std_catch
	
	/**
	 * Lists all entries of specified type
	 * \param d domain
	 * \param u user
	 * \param e extension
	 * \param uideq information read from database
	 * \param type type of entries
	 */
	cqmailvq::error * cqmailvq::user_conf_ls_by_type(const char * dom_id, 
			const char * _login, const char * _pfix,
			user_conf_type ut, user_conf_info_list_out ucis ) std_try {

		if( ! dom_id || ! _login || ! _pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		string login(text::lower(_login));
		string pfix(text::lower(_pfix));
		return auth->user_conf_ls_by_type( dom_id, login.c_str(), pfix.c_str(), 
				ut, ucis ); 
	} std_catch

	/**
	 * Changes entry
	 */
	cqmailvq::error * cqmailvq::user_conf_rep(const char * dom_id, 
			const char * _login, const char * _pfix,
			const user_conf_info & uin ) std_try {

		if( ! dom_id || ! _login || ! _pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		user_conf_info uio;
		uio.id_conf = uin.id_conf;
		auto_ptr<error> ret(user_conf_get(uio));
		if( ::vq::ivq::err_no != ret->ec )
				return ret.release();

		if( uin.type != uio.type )
				return lr(::vq::ivq::err_func_ni, "uin.type != uio.type");
	
		string lno, lnn;
		ret.reset(user_conf_ln(uio, lno));
		if( ::vq::ivq::err_no != ret->ec && ::vq::ivq::err_func_ni != ret->ec )
				return ret.release();
		ret.reset(user_conf_ln(uin, lnn));
		if( ::vq::ivq::err_no != ret->ec && ::vq::ivq::err_func_ni != ret->ec )
				return ret.release();

		if( ::vq::ivq::err_func_ni == ret->ec ) {
				return auth->user_conf_rep(uin);
		}

		string df(dotfile(dom_id, text::lower(_login), text::lower(_pfix)));
	
		// is it portable?? may be on some systems it will block?
		opfstream dout;
		string dftmp;
		ret.reset(tmp_crt(df, dout, dftmp));
		if( ::vq::ivq::err_no != ret->ec ) 
				return ret.release(); 
		
		ipfstream din(df.c_str());
		if(din) {
				string cln;
	
				if( ::vq::ivq::uc_autoresp == uin.type )
						dout<<lnn<<endl;
	
				while(getline(din, cln)) {
						if(cln!=lno && !cln.empty()) {
								dout<<cln<<endl;
								if(!dout) { 
										unlink(dftmp.c_str());
										return lr(::vq::ivq::err_wr, dftmp);
								}
						}
				}
			
				if( ! ( ::vq::ivq::uc_autoresp == uin.type ) )
						dout<<lnn<<endl;
	
				if(din.bad()) {
						unlink(dftmp.c_str());
						return lr(::vq::ivq::err_rd, df);
				}
				din.clear();
		} else 
				dout<<lnn<<endl;
		dout.flush();
		if(!dout) {
				unlink(dftmp.c_str());
				return lr(::vq::ivq::err_wr, dftmp);
		}
	
		ret.reset(auth->user_conf_rep(uin));
		if( ::vq::ivq::err_no != ret->ec ) {
				unlink(dftmp.c_str());
				return ret.release();
		}
		if(rename(dftmp.c_str(), df.c_str())) {
				return lr(::vq::ivq::err_ren, dftmp);
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 * Get all configuration for domain
	 */
	cqmailvq::error * cqmailvq::user_conf_get(user_conf_info &ui) std_try {
		return auth->user_conf_get(ui);
	} std_catch

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

	/**
	 * Create line which will be add do dot-qmail
	 */
	cqmailvq::error * cqmailvq::user_conf_ln(const user_conf_info &ui, 
			string & ln ) std_try {

		ln.erase();
		if( ::vq::ivq::uc_redir == ui.type) {
				ln.reserve( strlen(ui.val)+2 );
				ln = '&';
				ln += ui.val;
				lr(::vq::ivq::err_no, "");
		}

		if( ::vq::ivq::uc_autoresp == ui.type ) {
				string::size_type pos, pos1;
				string tmp, tmp1;
				ln = "|";
				ln += this->home+"/bin/autoresp ";
				tmp1 = text::hex_from(static_cast<const char *>(ui.val));
	
				pos = tmp1.find('\0');
				if( pos == string::npos )
						throw logic_error("user_conf_ln: there's no message!");
				
				tmp = tmp1.substr(0, pos);
				ln += tmp;
				
				pos1 = tmp1.find('\0', ++pos);
				tmp = tmp1.substr(pos, pos1 - pos);
				ln += ' ';
				ln += text::to_hex(tmp.data(), tmp.length());
				
				if( pos1 != string::npos 
					&& (pos1 = tmp1.find('\0', (pos=pos1+1))) != string::npos ) {
						tmp = tmp1.substr(pos, pos1-pos);
						if( ! tmp.empty() ) {
								ln+=' ';
								ln+=text::to_hex(tmp.data(), tmp.length());
						}
				}
				return lr(::vq::ivq::err_no, "");
		}
		return lr(::vq::ivq::err_func_ni, "wrong user_conf_info.type");
	} std_catch

	/**
	 * Is mailbox has entry with this type
	 * \param dom domain
	 * \param user user
	 * \param ext extension
	 * \param type type to find
	 */
	cqmailvq::error * cqmailvq::user_conf_type_has(const char * dom_id, 
			const char * _login, const char * _pfix,
			user_conf_type type) std_try {

		if( ! dom_id || ! _login || ! _pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		string login(text::lower(_login));
		string pfix(text::lower(_pfix));
		return auth->user_conf_type_has( dom_id, login.c_str(), 
				pfix.c_str(), type);
	} std_catch

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

	/**
	 * Remove all entries of given type
	 * \param d domain
	 * \param u user
	 * \param e extension
	 * \param type delete this type
	 */
	cqmailvq::error * cqmailvq::user_conf_rm_by_type(const char * dom_id, 
			const char * _login, const char * _pfix,
			user_conf_type type) std_try {

		string login(text::lower(_login));
		string pfix(text::lower(_pfix));
	
		user_conf_info_list_var uis;
		auto_ptr<error> ret(user_conf_ls_by_type(dom_id, login.c_str(), 
				pfix.c_str(), type, uis));
		if( ::vq::ivq::err_no != ret->ec ) 
				return ret.release();
		
		string ln;
		CORBA::ULong uisl = uis->length(), i;
		for( i=0; i<uisl; ++i ) {
				ret.reset(user_conf_ln(uis[i], ln));
				if( ::vq::ivq::err_func_ni == ret->ec ) {
						return auth->user_conf_rm_by_type(dom_id, login.c_str(),
								pfix.c_str(), type );
				} else if( ::vq::ivq::err_no != ret->ec ) {
						return ret.release();		
				}
				uis[i].val = ln.c_str();
		}
	
		// is it portable?? may be on some systems it will block?
		opfstream dout;
		string dftmp;
		string df(dotfile(dom_id, login, pfix));
		ret.reset(tmp_crt(df, dout, dftmp));
		if( ::vq::ivq::err_no != ret->ec ) 
				return ret.release();
		
		ipfstream din(df.c_str());
		if(din) {
				string cln;
				while(getline(din, cln)) {
						if( cln.empty() ) continue;
						for( i = 0; i<uisl; ++i ) {
								if( cln == static_cast<const char *>
										(uis[i].val) ) break;
						}
						if( i == uisl ) {
								dout<<cln<<endl;
								if(!dout) {
										unlink(dftmp.c_str());
										return lr(::vq::ivq::err_wr, dftmp);
								}
						}
				}
				if(din.bad()) {
						unlink(dftmp.c_str());
						return lr(::vq::ivq::err_rd, df);
				}
				din.clear();
		}
	
		dout.flush();
		if(!dout) {
				unlink(dftmp.c_str());
				return lr(::vq::ivq::err_wr, dftmp);
		}
		ret.reset(auth->user_conf_rm_by_type( dom_id, login.c_str(), 
				pfix.c_str(), type));
		if( ::vq::ivq::err_no != ret->ec ) {
				unlink(dftmp.c_str());
				return ret.release();
		}
		if( rename(dftmp.c_str(), df.c_str()) ) {
				return lr(::vq::ivq::err_ren, dftmp);
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Count entries of this type
	 * \param d domain
	 * \param u user
	 * \param e extension
	 * \param type
	 * \param cnt number of entries found
	 */
	cqmailvq::error * cqmailvq::user_conf_type_cnt( const char * dom_id, 
			const char * _login, const char * _pfix,
			user_conf_type ut, size_type &cnt) std_try {

		if( ! dom_id || ! _login || ! _pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		string login(text::lower(_login));
		string pfix(text::lower(_pfix));
		return auth->user_conf_type_cnt( dom_id, login.c_str(), pfix.c_str(),
			ut, cnt);
	} std_catch
} // namespace POA_vq
