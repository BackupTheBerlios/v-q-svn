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
#include "cpgsqlauth.hpp"

#include <text.hpp>
#include <util.hpp>

#include <stdexcept>

namespace POA_vq {
	
	using namespace std;
	using namespace pqxx;
	using namespace text;
	using sys::str2tb;

	#define std_try { try
	#define std_catch catch( sql_error & e ) { \
		string em("sql error: \""); \
		em+=e.query()+"\": "+e.what(); \
		cout<<em<<endl; \
		throw ::vq::db_error(em.c_str(), __FILE__, __LINE__); \
	} catch( std::exception & e ) { \
		cout<<"exception: "<<e.what()<<endl; \
		throw ::vq::except(e.what(), __FILE__, __LINE__); \
	} }
	
	/**
	 * \param pginfo Connection configuration
	 */
	cpgsqlauth::cpgsqlauth( const char *pginfo ) std_try {
		pg.reset(new Connection(pginfo));
		if( ! pg.get() || ! pg->is_open() ) {
				throw ::vq::except(
						static_cast<const char*>("can't create connection"),
						__FILE__, __LINE__ ); 
		}
	} std_catch;
	
	/**
	 *
	 */
	cpgsqlauth::~cpgsqlauth() std_try {
	} std_catch
	
	/**
	*/
	cpgsqlauth::error cpgsqlauth::dom_add( const char * _dom ) std_try {
		if( ! _dom ) throw ::vq::null_error(__FILE__, __LINE__);

		string dom(lower(_dom));

		Result res(NonTransaction(*pg).Exec(
			"SELECT DOM_ADD("+Quote(dom, false)+")"));

		if(res.empty() || res[0][0].is_null() ) {
				return ::vq::iauth::err_func_res;
		}

		const char *val = res[0][0].c_str();
		if( *val && *(val+1) == '\0' ) {
				if( *val == '2' ) {
						return ::vq::iauth::err_exists;
				} else if( *val == '1' ) {
						return ::vq::iauth::err_no;
				}
		}

		return ::vq::iauth::err_func_res;
	} std_catch
	
	/**
	*/
	cpgsqlauth::error cpgsqlauth::dom_rm( const char * _dom ) std_try {
		if( ! _dom ) throw ::vq::null_error(__FILE__, __LINE__);
	
		string dom(lower(_dom));
		NonTransaction(*pg).Exec("SELECT DOM_RM("+Quote(dom, false)+")");

		return ::vq::iauth::err_no;
	} std_catch
	
	/**
	*/
	cpgsqlauth::error cpgsqlauth::user_add( const auth_info & ai ) std_try {
		if( !ai.dom || !ai.user || !ai.pass || !ai.dir )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		string dom(lower(str2tb(string(ai.dom))));
		string user(lower(string(ai.user)));
	
		Result res(NonTransaction(*pg).Exec(
				"SELECT USER_ADD("+Quote(user, false)+","
				+Quote(dom, false)+","
				+Quote(string(ai.pass), false)+","
				+Quote(ToString(ai.flags), false)+")"));
	
		if(res.empty() || res[0][0].is_null() ) {
				return ::vq::iauth::err_func_res;
		}

		const char *val = res[0][0].c_str();
		if( *(val+1) == '\0' ) {
				if( *val == '1' ) {
						return ::vq::iauth::err_user_inv;
				} else if( *val == '0' ) {
						return ::vq::iauth::err_no;
				}
		}
		return ::vq::iauth::err_func_res;
	} std_catch;
	
	/**
	 * 
	 */
	cpgsqlauth::error cpgsqlauth::user_pass( const char *_dom, 
			const char *_user, const char *_pass ) std_try {
		if( ! _dom || ! _user || ! _pass )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		string dom(lower(str2tb(_dom)));
		string user(lower(_user));
	
		Result res(NonTransaction(*pg).Exec(
				"SELECT USER_PASS("+Quote(user, false)+","
				+Quote(dom, false)+","+Quote(_pass, false)+")"));

		if(res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "t") ) {
				return ::vq::iauth::err_func_res;
		}
		return ::vq::iauth::err_no;
	} std_catch
	
	/**
	*/
	cpgsqlauth::error cpgsqlauth::user_rm( const char *_dom, 
			const char *_user) std_try {
		if( ! _dom || ! _user )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		string dom(lower(str2tb(_dom)));
		string user(lower(_user));
		Result res(NonTransaction(*pg).Exec(
				"SELECT USER_RM("+Quote(user, false)+","
				+Quote(dom, false)+")"));

		if(res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "t") ) {
				return ::vq::iauth::err_func_res;
		}
		return ::vq::iauth::err_no;
	} std_catch
#if 0
	/**
	*/
	cpgsqlauth::error cpgsqlauth::user_auth() std_try {
		if( fdrdstr(cso, user) != -1
		   && fdrdstr(cso, dom) != -1 ) {
				dom = lower(dom);
				user = lower(user);
				alias2dom(*pg, dom);
				string tb(str2tb(dom));
	
				Result res(NonTransaction(*pg).Exec(
					"SELECT pass,flags FROM \""+tb
					+"\" WHERE login="+Quote(user, false)));
	
				if( res.size() == 1 ) {
						if( res[0][0].is_null() || res[0][1].is_null() ) {
								if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
								return;
						} std_catch
	
						const char *pass = res[0][0].c_str();
						if(!pass) pass = "";
						const char *flags = res[0][1].c_str();
						if(!flags) flags= "0";
						istringstream is;
						is.str(flags);
						int iflags;
						is>>iflags;
						if( ! is ) {
								if( fdwrite(cso, &"E", 1) != 1 ) throw wr_error();
								return;
						} std_catch
						if( fdwrite(cso,&"F",1) != 1
							|| fdwrstr(cso, user) == -1
							|| fdwrstr(cso, dom) == -1
							|| fdwrstr(cso, pass) == -1
							|| fdwrite(cso, &iflags, sizeof(iflags)) == -1 ) 
								throw wr_error();
				} else if(fdwrite(cso,&"Z",1)!=1) throw wr_error();
		} else throw rd_error();
	} std_catch
	
	/**
	 */
	cpgsqlauth::error cpgsqlauth::user_ex() std_try {
		if( -1 != fdrdstr(cso, dom)
			&& -1 != fdrdstr(cso, user) ) {
				uint8_t ret=0xff;
				dom = lower(dom);
				user = lower(user);
				alias2dom(*pg, dom);
				dom = str2tb(dom);
	
				Result res(NonTransaction(*pg).Exec(
					"SELECT user_exists("+Quote(dom, false)+","
					+Quote(user, false)+')'));
	
				if( res.size() == 1 ) {
						istringstream is;
						unsigned ret1;
						const char *tmp = res[0][0].c_str();
						if(!tmp) tmp = "";
						is.str(tmp);
						is>>ret1;
						if(is) ret = ret1 & 0xff;
				} std_catch
				if( 1 != fdwrite(cso, &"F", 1) ) {
						switch(ret) {
						case 1:
								ret = cvq::err_user_nf;
								break;
						case 0:
								ret = cvq::err_no;
								break;
						case 2:
								ret = cvq::err_dom_inv;
								break;
						default:
								ret = cvq::err_temp;
						} std_catch
					
						if( sizeof(ret) == fdwrite(cso, &ret, sizeof(ret)) )
								return;
				} std_catch
				throw wr_error();
		} else throw rd_error();
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::dom_ip_add() std_try {
		if( fdrdstr(cso,dom) != -1
			&& fdrdstr(cso,ip) != -1 ) {
				Result res(NonTransaction(*pg).Exec(
					"INSERT INTO ip2domain (DOMAIN,IP) VALUES("
					+Quote(lower(dom), false)+","
					+Quote(lower(ip), false)+')'));
	
				if(fdwrite(cso,&"K",1) != 1) throw wr_error();
		} else throw rd_error();
	} std_catch
	
	/**
	*/
	cpgsqlauth::error cpgsqlauth::dom_ip_rm() std_try {
		if( fdrdstr(cso,dom) != -1
			&& fdrdstr(cso,ip) != -1 ) {
				Result res(NonTransaction(*pg).Exec(
					"DELETE FROM ip2domain WHERE DOMAIN="
					+Quote(lower(dom), false)
					+" AND IP="+Quote(lower(ip), false)));
	
				if(fdwrite(cso,&"K",1) != 1) throw wr_error();
		} else throw rd_error();
	} std_catch
	/**
	*/
	cpgsqlauth::error cpgsqlauth::dom_ip_rm_all() std_try {
		if( fdrdstr(cso,dom) != -1 ) {
				Result res(NonTransaction(*pg).Exec(
					"DELETE FROM ip2domain WHERE DOMAIN="
					+Quote(lower(dom), false)));
	
				if(fdwrite(cso,&"K",1) != 1) throw wr_error();
		} else throw rd_error();
	} std_catch
	/**
	 * 
	 */
	cpgsqlauth::error cpgsqlauth::dom_ip_ls() std_try {
		if( fdrdstr(cso,dom) == -1 )
				throw rd_error();
	
		dom = lower(dom);
		
		Result res(NonTransaction(*pg).Exec(
			"SELECT IP FROM ip2domain WHERE DOMAIN="
			+Quote(dom, false)+" ORDER BY IP"));
	
		vector<string>::size_type i, cnt = res.size();
		if( fdwrite(cso,&"F",1) == -1
			|| fdwrite(cso,&cnt,sizeof(cnt)) == -1 )
				throw wr_error();
	
		string::size_type ipl;
		const char *ip;
		for(i=0; i<cnt; ++i) {
				ip = res[i][0].c_str();
				ipl = ! ip ? 0 : strlen(ip);
				if( fdwrite(cso,&ipl,sizeof(ipl)) == -1
					|| (ip && fdwrite(cso, ip, ipl) == -1) )
						throw wr_error();
		} std_catch
	} std_catch
	/**
	*/
	cpgsqlauth::error cpgsqlauth::dom_ip_ls_dom() std_try {
		Result res(NonTransaction(*pg).Exec(
			"SELECT DISTINCT DOMAIN FROM ip2domain ORDER BY DOMAIN"));
	
		vector<string>::size_type i, cnt = res.size();
		if(fdwrite(cso,&"F",1) == -1
		   || fdwrite(cso,&cnt,sizeof(cnt)) == -1 )
				throw wr_error();
	
		string::size_type doml;
		const char *dom;
		for(i=0; i<cnt; ++i) {
				dom = res[i][0].c_str();
				doml= !dom ? 0 : strlen(dom);
				if( fdwrite(cso,&doml,sizeof(doml)) == -1
					|| (dom && fdwrite(cso, dom, doml) == -1) )
						throw wr_error();
		} std_catch
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_add() throw (out_of_range) std_try {
		string val;
		if( fdrdstr(cso, dom) == -1
		   || fdrdstr(cso, user) == -1
		   || fdrdstr(cso, ext) == -1
		   || fdread(cso, &type, sizeof(type)) == -1 
		   || fdrdstr(cso, val) == -1 )
				throw rd_error();
		
		dom = lower(dom);
		user = lower(user);
		ext = lower(ext);
		
		dom = str2tb(dom);
		Result res(NonTransaction(*pg).Exec(
			"SELECT UDOT_ADD("+Quote(dom, false)+","+Quote(user, false)
			+","+Quote(ext, false)+",'\\"+(char)type+"',"
			+Quote(val, false)+")"));
	
		if(res.size() && !res[0][0].is_null()) {
				string id(res[0][0].c_str());
				if(fdwrite(cso,&"F",1) == -1 || fdwrstr(cso, id) == -1 )
						throw wr_error();
				return;
		} std_catch
		if(fdwrite(cso,&"E",1)!=1) throw wr_error();
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_ls() std_try {
		string ext;
		if( fdrdstr(cso, dom) == -1
		   || fdrdstr(cso, user) == -1
		   || fdrdstr(cso, ext) == -1 )
				throw rd_error();
		
		dom = lower(str2tb(dom));
		user = lower(user);
		ext = lower(ext);
		Result res(NonTransaction(*pg).Exec(
			"SELECT ID,TYPE,VALUE FROM \""+dom+"_dot\" WHERE EXT="
			+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
			+" ORDER BY ID"));
	
		vector<string>::size_type i, cnt = res.size();
		if(fdwrite(cso,&"F",1) == -1
		   || fdwrite(cso,&cnt,sizeof(cnt)) == -1 )
				throw wr_error();
	
		const char *tmp;
		cvq::udot_info ui;
		for(i=0; i<cnt; ++i) {
				tmp = res[i][0].c_str();
				ui.id = !tmp ? "" : tmp;
				tmp = res[i][1].c_str();
				(char)ui.type = !tmp ? 0 : *tmp;
				tmp = res[i][2].c_str();
				ui.val = !tmp ? "" : tmp;
	
				if( fdwrstr(cso,ui.id) == -1 
					|| fdwrite(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
					|| fdwrstr(cso, ui.val) == -1 )
						throw wr_error();
		} std_catch
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_ls_type() std_try {
		if( fdrdstr(cso, dom) == -1
		   || fdrdstr(cso, user) == -1
		   || fdrdstr(cso, ext) == -1
		   || fdread(cso, &type, sizeof(type)) == -1 )
				throw rd_error();
	
		dom = lower(str2tb(dom));
		user = lower(user);
		ext = lower(ext);
		Result res(NonTransaction(*pg).Exec(
			"SELECT ID,TYPE,VALUE FROM \""+dom+"_dot\" WHERE EXT="
			+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
			+" AND TYPE='\\"+(char)type+"' ORDER BY ID"));
	
		vector<string>::size_type i, cnt = res.size();
		if(fdwrite(cso,&"F",1) == -1
		   || fdwrite(cso,&cnt,sizeof(cnt)) == -1 )
				throw wr_error();
	
		const char *tmp;
		cvq::udot_info ui;
		for(i=0; i<cnt; ++i) {
				tmp = res[i][0].c_str();
				ui.id = !tmp ? "" : tmp;
				tmp = res[i][1].c_str();
				(char)ui.type = !tmp ? 0 : *tmp;
				tmp = res[i][2].c_str();
				ui.val = !tmp ? "" : tmp;
				if( fdwrstr(cso,ui.id) == -1 
					|| fdwrite(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
					|| fdwrstr(cso, ui.val) == -1 )
						throw wr_error();
		} std_catch
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_rm() std_try {
		string id;
		if( fdrdstr(cso, dom) == -1
		   || fdrdstr(cso, id) == -1 )
				throw rd_error();
	
		dom = lower(str2tb(dom));
		Result res(NonTransaction(*pg).Exec(
			"DELETE FROM \""+dom+"_dot\" WHERE ID="+Quote(id, false)));
	
		if(fdwrite(cso, &"K", 1)!=1) throw wr_error();
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_rm_type() std_try {
		if( fdrdstr(cso, dom) == -1 
			|| fdrdstr(cso, user) == -1 
			|| fdrdstr(cso, ext) == -1
			|| fdread(cso, &type, sizeof(type)) != sizeof(type) )
				throw rd_error();
	
		dom = lower(str2tb(dom));
		user = lower(user);
		ext = lower(ext);
		Result res(NonTransaction(*pg).Exec(
			"DELETE FROM \""+dom+"_dot\" WHERE EXT="
			+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
			+" AND TYPE='\\"+(char)type+"'"));
	
		if(fdwrite(cso, &"K",1) != 1) throw wr_error();
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_get() std_try {
		string id;
		if( fdrdstr(cso, dom) == -1
		   || fdrdstr(cso, id) == -1 )
				throw rd_error();
		
		dom = lower(str2tb(dom));
		Result res(NonTransaction(*pg).Exec(
			"SELECT TYPE,VALUE FROM \""+dom+"_dot\" WHERE ID="+Quote(id, false)));
	
		if(res.empty()) {
				if(fdwrite(cso, &"Z", 1)!=1) throw wr_error();
				return;
		} std_catch
		
		string val;
		const char *ptr;
		ptr = res[0][0].c_str();
		(char)type = ! ptr ? '\0' : *ptr;
		ptr = res[0][1].c_str();
		val = ! ptr ? "" : ptr;
		
		if( fdwrite(cso, &"F", 1) != 1 
		   || fdwrite(cso, &type, sizeof(type)) != sizeof(type)
		   || fdwrstr(cso, val) == -1 )
				throw wr_error();
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_rep() std_try {
		cvq::udot_info ui;
	
		if( fdrdstr(cso, dom) == -1
		   || fdrdstr(cso, ui.id) == -1
		   || fdread(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
		   || fdrdstr(cso, ui.val) == -1 )
				throw rd_error();
	
		dom = lower(str2tb(dom));
		Result res(NonTransaction(*pg).Exec(
			"UPDATE \""+dom+"_dot\" SET TYPE='\\"+(char)ui.type
			+"',VALUE="+Quote(ui.val, false)+" WHERE ID="
			+Quote(ui.id, false)));
	
		if( ! res.AffectedRows() ) {
				if(fdwrite(cso, &"Z", 1) != 1) throw wr_error();
		} else 
				if(fdwrite(cso, &"K", 1) != 1) throw wr_error();
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_has() std_try {
		if( fdrdstr(cso, dom) == -1
		   || fdrdstr(cso, user) == -1
		   || fdrdstr(cso, ext) == -1
		   || fdread(cso, &type, sizeof(type)) == -1 )
				throw rd_error();
	
		dom = lower(str2tb(dom));
		user = lower(user);
		ext = lower(ext);
		Result res(NonTransaction(*pg).Exec(
			"SELECT 1 FROM \""+dom+"_dot\" WHERE ext="
			+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
			+" AND TYPE='\\"+(char)type+"' LIMIT 1"));
	
		if( ! res.empty() ) {
				if(fdwrite(cso, &"T", 1) != 1) throw wr_error();
		} else 
				if(fdwrite(cso, &"F",1) != 1) throw wr_error();
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::udot_type_cnt() std_try {
		if( fdrdstr(cso, dom) == -1 
			|| fdrdstr(cso, user) == -1 
			|| fdrdstr(cso, ext) == -1
			|| fdread(cso, &type, sizeof(type)) != sizeof(type) )
				throw rd_error();
		
		dom = lower(str2tb(dom));
		user = lower(user);
		ext = lower(ext);
		Result res(NonTransaction(*pg).Exec(
			"SELECT COUNT(*) FROM \""+dom+"_dot\" WHERE ext="
			+Quote(user+ac_qmail_hyp.val_str().at(0)+ext, false)
			+" AND TYPE='\\"+(char)type+"'"));
	
		if( res.empty() || res[0][0].is_null() ) {
				if( fdwrite(cso, &"E", 1)) throw wr_error();
		} std_catch
	
		const char *ptr = res[0][0].c_str();
	
		cdaemonauth::size_type cnt;
		istringstream is(ptr);
		is>>cnt;
		if( ! is ) cnt = 0;
		
		if(fdwrite(cso, &"F",1) != 1
		   || fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt))
				throw wr_error();
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::qt_def_set( const string & d ) std_try {
		dom = lower(d);
		if( -1 != fdread(cso, &qb, sizeof qb)
			&& -1 != fdread(cso, &qf, sizeof qf) ) {
				dom = lower(dom);
	
				ostringstream qb_str, qf_str;
				qb_str<<qb;
				qf_str<<qf;
				
				Result res(NonTransaction(*pg).Exec(
					"SELECT qt_def_set("
					+Quote(dom, false)+","
					+Quote(qb_str.str(), false)+","
					+Quote(qf_str.str(), false)+')'));
	
				if( fdwrite(cso, &"K", 1) != 1 ) 
						throw wr_error();
		} else throw rd_error();
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error cpgsqlauth::qt_global_def_set() std_try {
		qt_def_set("-");
	} std_catch
	
	/**
	 * Gets default quota for new users and sends it via cso
	 * \param dom domain
	 */
	cpgsqlauth::error cpgsqlauth::qt_def_get( const string & d ) std_try {
		qb = qf = 0;
		dom = lower(d);
		Result res(NonTransaction(*pg).Exec(
			"SELECT qt_user_bytes_def,qt_user_files_def FROM domains"
			" WHERE name="+Quote(dom, false)+" LIMIT 1"));
	
		if( res.size() <= 0 ) {
				if( fdwrite(cso, &"Z", 1) != 1 )
						throw wr_error();
				return;
		} std_catch
		
		if( ! res[0][0].is_null() )
				res[0][0].to<cauth::quota_value>(qb);
		if( ! res[0][1].is_null() )
				res[0][1].to<cauth::quota_value>(qf);
	
		if( fdwrite(cso, &"F", 1) != 1
			|| fdwrite(cso, &qb, sizeof qb) != sizeof qb
			|| fdwrite(cso, &qf, sizeof qf) != sizeof qf) 
				throw wr_error();
	} std_catch
	
	/**
	 * Gets limits for specified user
	 */
	cpgsqlauth::error cpgsqlauth::qt_get() std_try {
		if( -1 == fdrdstr(cso, dom)
			|| -1 == fdrdstr(cso, user))
				throw rd_error();
	
		qb = qf = 0;
		dom = lower(str2tb(dom));
		user = lower(user);
	
		Result res(NonTransaction(*pg).Exec(
			"SELECT qt_bytes,qt_files FROM \""+dom
			+"\" WHERE login="+Quote(user, false)));
	
		if( res.size() <= 0 ) {
				if( fdwrite(cso, &"Z", 1) != 1 )
						throw wr_error();
				return;
		} std_catch
		
		if( ! res[0][0].is_null() )
				res[0][0].to<cauth::quota_value>(qb);
		if( ! res[0][1].is_null() )
				res[0][1].to<cauth::quota_value>(qf);
	
		if( fdwrite(cso, &"F", 1) != 1
			|| fdwrite(cso, &qb, sizeof qb) != sizeof qb
			|| fdwrite(cso, &qf, sizeof qf) != sizeof qf) 
				throw wr_error();
	} std_catch
	
	/**
	 * Sets limits for specified user
	 */
	cpgsqlauth::error cpgsqlauth::qt_set() std_try {
		if( -1 == fdrdstr(cso, dom)
			|| -1 == fdrdstr(cso, user)
			|| -1 == fdread(cso, &qb, sizeof qb)
			|| -1 == fdread(cso, &qf, sizeof qf) )
				throw rd_error();
	
		dom = str2tb(lower(dom));
		user = lower(user);
	
		Result res(NonTransaction(*pg).Exec(
			"UPDATE \""+dom+"\" SET qt_bytes="+ToString<cauth::quota_value>(qb)
			+",qt_files="+ToString<cauth::quota_value>(qf)
			+" WHERE login="+Quote(user, false)));
	
		if( fdwrite(cso, &"K", 1) != 1 ) 
				throw wr_error();
	} std_catch
	
	/**
	 */
	cpgsqlauth::error cpgsqlauth::qt_global_def_get() std_try {
		qt_def_get("-");
	} std_catch
	
	/**
	 */
	cpgsqlauth::error cpgsqlauth::qt_def_get() std_try {
		if( -1 != fdrdstr(cso, dom) ) {
				qt_def_get(dom);
		} else throw rd_error();
	} std_catch
	
	/**
	 */
	cpgsqlauth::error cpgsqlauth::qt_def_set() std_try {
		if( -1 != fdrdstr(cso, dom) ) {
				qt_def_set(dom);
		} else throw rd_error();
	} std_catch
#endif // if 0	
} // namespace POA_vq
