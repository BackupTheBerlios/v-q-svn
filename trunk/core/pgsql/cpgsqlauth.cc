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
	cpgsqlauth::error * cpgsqlauth::dom_add( const char * _dom,
			CORBA::String_out dom_id ) std_try {
		if( ! _dom ) throw ::vq::null_error(__FILE__, __LINE__);

		string dom(lower(_dom));

		Result res(NonTransaction(*pg).Exec(
			"SELECT DOM_ADD("+Quote(dom, false)+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "DOM_ADD");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_exists, dom) 
						: lr(::vq::ivq::err_func_res, "DOM_ADD");
						
		}
		dom_id = val;
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_id( const char * _dom,
			CORBA::String_out dom_id ) std_try {
		if( ! _dom ) throw ::vq::null_error(__FILE__, __LINE__);

		string dom(lower(_dom));

		Result res(NonTransaction(*pg).Exec(
			"SELECT DOM_ID("+Quote(dom, false)+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "DOM_ID");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_noent, dom) 
						: lr(::vq::ivq::err_func_res, "DOM_ID");
						
		}
		dom_id = val;
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_name( const char * dom_id,
			CORBA::String_out domain ) std_try {
		if( ! dom_id ) throw ::vq::null_error(__FILE__, __LINE__);

		Result res(NonTransaction(*pg).Exec(
			"SELECT DOM_NAME("+Quote(dom_id, false)+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "DOM_NAME");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_noent, dom_id) 
						: lr(::vq::ivq::err_func_res, "DOM_NAME");
						
		}
		domain = val;
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_rm( const char * dom_id ) std_try {
		if( ! dom_id ) throw ::vq::null_error(__FILE__, __LINE__);
	
		NonTransaction(*pg).Exec("SELECT DOM_RM("+Quote(dom_id, false)+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	*/
	cpgsqlauth::error * cpgsqlauth::user_add( const auth_info & ai, 
			CORBA::Boolean is_banned ) std_try {
		if( !ai.id_domain || !ai.login || !ai.pass /*|| !ai.dir*/ )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
				"SELECT USER_ADD("
				+Quote(static_cast<const char *>(ai.id_domain), false)+','
				+Quote(lower(static_cast<const char *>(ai.login)), false)+','
				+Quote(static_cast<const char *>(ai.pass), false)+','
				+Quote(ToString(ai.flags), false)+','
				+(is_banned ? "'t'" : "'f'")+"::boolean)"));
	
		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "USER_ADD");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				switch( *(val+1) ) {
				case '2':
						return lr(::vq::ivq::err_user_inv, "");
				case '1':
						return lr(::vq::ivq::err_exists, "");
				default:
						return lr(::vq::ivq::err_func_res, "USER_ADD");
				} 
		}
		if( *val == '0' && *(val+1) == '\0' ) {
				return lr(::vq::ivq::err_no, "");
		}
		return lr(::vq::ivq::err_func_res, "USER_ADD");
	} std_catch;

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dra_add( const char* dom_id, 
			const char* rea ) std_try {
		if( ! dom_id || ! rea )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
				"SELECT DRA_ADD("
				+Quote(static_cast<const char *>(dom_id))+','
				+Quote(static_cast<const char *>(rea))+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "DRA_ADD");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_exists, "")
						: lr(::vq::ivq::err_func_res, "DRA_ADD");
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dra_rm( const char* dom_id, 
			const char* rea ) std_try {
		if( ! dom_id || ! rea )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		NonTransaction(*pg).Exec(
				"SELECT DRA_RM("
				+Quote(static_cast<const char *>(dom_id))+','
				+Quote(static_cast<const char *>(rea))+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dra_rm_by_dom( const char* dom_id ) std_try {
		if( ! dom_id )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		NonTransaction(*pg).Exec(
				"SELECT DRA_RM_BY_DOM("
				+Quote(static_cast<const char *>(dom_id))+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dra_ls_by_dom( const char* dom_id, 
			string_list_out reas ) std_try {

		Result res(NonTransaction(*pg).Exec("SELECT * FROM dra_ls_by_dom("
			+Quote(static_cast<const char *>(dom_id))+')'));
		if(res.empty()) return lr(::vq::ivq::err_no, "");

		Result::size_type s = res.size();
		reas = new string_list(static_cast<CORBA::ULong>(s));
		reas->length(static_cast<CORBA::ULong>(s));
		for( Result::size_type i=0; i<s; ++i ) {
			reas[i] = CORBA::string_dup(
				res[i][0].is_null() ? "" : res[i][0].c_str());
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 * 
	 */
	cpgsqlauth::error * cpgsqlauth::user_pass( const char *dom_id, 
			const char *login, const char *_pass ) std_try {
		if( ! dom_id || ! login || ! _pass )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
				"SELECT USER_PASS("
				+Quote(dom_id, false)+','
				+Quote(lower(login), false)+','
				+Quote(_pass, false)+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "USER_PASS");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_noent, "")
						: lr(::vq::ivq::err_func_res, "USER_PASS");
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	*/
	cpgsqlauth::error * cpgsqlauth::user_rm( const char *dom_id, 
			const char *login) std_try {
		if( ! dom_id || ! login )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		NonTransaction(*pg).Exec(
				"SELECT USER_RM("
				+Quote(dom_id, false)+','
				+Quote(lower(login), false)+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::user_ex( const char * dom_id,
			const char * login ) std_try {
		if( ! dom_id || ! login ) throw ::vq::null_error(__FILE__, __LINE__);

		Result res(NonTransaction(*pg).Exec(
			"SELECT USER_EX("
			+Quote(dom_id, false)+','
			+Quote(lower(login), false)+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "USER_EX");
		}

		const char *val = res[0][0].c_str();
		if( *val && '\0' == *(val+1) ) {
				switch(*val) {
				case '0': return lr(::vq::ivq::err_no, "");
				case '1': return lr(::vq::ivq::err_noent, "");
				}
		}
		return lr(::vq::ivq::err_func_res, "USER_EX");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::eb_add( const char * re_domain,
			const char * re_login ) std_try {
		if( ! re_domain || ! re_login )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
				"SELECT EB_ADD("
				+Quote(static_cast<const char *>(re_domain))+','
				+Quote(static_cast<const char *>(re_login))+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "EB_ADD");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_exists, "")
						: lr(::vq::ivq::err_func_res, "EB_ADD");
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::eb_rm( const char * re_domain,
			const char * re_login ) std_try {
		if( ! re_domain || ! re_login )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		NonTransaction(*pg).Exec(
				"SELECT EB_RM("
				+Quote(static_cast<const char *>(re_domain))+','
				+Quote(static_cast<const char *>(re_login))+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::eb_ls( email_banned_list_out ebs ) std_try {
		Result res(NonTransaction(*pg).Exec(
			"SELECT re_domain,re_login FROM eb_ls()"));
		if(res.empty()) return lr(::vq::ivq::err_no, "");

		Result::size_type s = res.size();
		ebs = new email_banned_list(static_cast<CORBA::ULong>(s));
		ebs->length(static_cast<CORBA::ULong>(s));
		for( Result::size_type i=0; i<s; ++i ) {
			ebs[i].re_domain = CORBA::string_dup(
				res[i][0].is_null() ? "" : res[i][0].c_str());
			ebs[i].re_login = CORBA::string_dup(
				res[i][1].is_null() ? "" : res[i][1].c_str());
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	

	/**
	 * 
	 */
	cpgsqlauth::error * cpgsqlauth::user_auth( auth_info & ai ) std_try {
		if( ! ai.id_domain || ! ai.login )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
			"SELECT user_auth("
			+Quote(static_cast<const char *>(ai.id_domain))+','
			+Quote(lower(static_cast<const char *>(ai.login)))+','
			+Quote(static_cast<const char *>(ai.pass))+')' ));
		
		if( res.empty() || res[0][0].is_null() ) 
				return lr(::vq::ivq::err_func_res, "USER_AUTH");

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				switch( *(val+1) ) {
				case '2':
						return lr(::vq::ivq::err_pass_inv, "");
				case '1':
						return lr(::vq::ivq::err_noent, "");
				default:
						return lr(::vq::ivq::err_func_res, "USER_AUTH");
				} 
		}

		ai.flags = res[0][0].as< ::vq::iauth::aif_type >(0);
		return lr(::vq::ivq::err_no, "");
	} std_catch
#if 0	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dom_ip_add() std_try {
		if( fdrdstr(cso,dom) != -1
			&& fdrdstr(cso,ip) != -1 ) {
				Result res(NonTransaction(*pg).Exec(
					"INSERT INTO ip2domain (DOMAIN,IP) VALUES("
					+Quote(lower(dom), false)+','
					+Quote(lower(ip), false)+')'));
	
				if(fdwrite(cso,&'K',1) != 1) throw wr_error();
		} else throw rd_error();
	} std_catch
	
	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_ip_rm() std_try {
		if( fdrdstr(cso,dom) != -1
			&& fdrdstr(cso,ip) != -1 ) {
				Result res(NonTransaction(*pg).Exec(
					"DELETE FROM ip2domain WHERE DOMAIN="
					+Quote(lower(dom), false)
					+" AND IP="+Quote(lower(ip), false)));
	
				if(fdwrite(cso,&'K',1) != 1) throw wr_error();
		} else throw rd_error();
	} std_catch
	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_ip_rm_all() std_try {
		if( fdrdstr(cso,dom) != -1 ) {
				Result res(NonTransaction(*pg).Exec(
					"DELETE FROM ip2domain WHERE DOMAIN="
					+Quote(lower(dom), false)));
	
				if(fdwrite(cso,&'K',1) != 1) throw wr_error();
		} else throw rd_error();
	} std_catch
	/**
	 * 
	 */
	cpgsqlauth::error * cpgsqlauth::dom_ip_ls() std_try {
		if( fdrdstr(cso,dom) == -1 )
				throw rd_error();
	
		dom = lower(dom);
		
		Result res(NonTransaction(*pg).Exec(
			"SELECT IP FROM ip2domain WHERE DOMAIN="
			+Quote(dom, false)+" ORDER BY IP"));
	
		vector<string>::size_type i, cnt = res.size();
		if( fdwrite(cso,&'F',1) == -1
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
	cpgsqlauth::error * cpgsqlauth::dom_ip_ls_dom() std_try {
		Result res(NonTransaction(*pg).Exec(
			"SELECT DISTINCT DOMAIN FROM ip2domain ORDER BY DOMAIN"));
	
		vector<string>::size_type i, cnt = res.size();
		if(fdwrite(cso,&'F',1) == -1
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
#endif // if 0
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_add( const char * dom_id,
			const char *login, const char * pfix, 
			udot_info &ui ) std_try {

		if( ! dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
			"SELECT UDOT_ADD("
			+ Quote(dom_id, false)+','
			+ Quote(lower(login), false)+','
			+ Quote(lower(pfix), false)+','
			+ Quote(ToString( static_cast<CORBA::ULong>(ui.type) ))+','
			+ Quote(static_cast<const char *>(ui.val), false)+')'));
	
		if(res.empty() || res[0][0].is_null()) {
				return lr(::vq::ivq::err_func_res, "UDOT_ADD");
		}
		ui.id_conf = res[0][0].c_str();
		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_ls(const char *dom_id, 
			const char * login, const char * pfix, 
			udot_info_list_out uis) std_try {
		
		if( !dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		Result res(NonTransaction(*pg).Exec(
			"SELECT id_conf,val,type FROM udot_ls("
			+ Quote(dom_id, false)+','
			+ Quote(lower(login), false)+','
			+ Quote(lower(pfix), false)+')'));

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		Result::size_type s = res.size();
		uis = new udot_info_list(static_cast<CORBA::ULong>(s));
		uis->length(static_cast<CORBA::ULong>(s));
		for( Result::size_type i=0; i<s; ++i ) {
			uis[i].id_conf = res[i][0].is_null() 
				? static_cast<const char *>("") : res[i][0].c_str();
			uis[i].val = res[i][1].is_null() 
				? static_cast<const char *>("") : res[i][1].c_str();
			uis[i].type = static_cast<udot_type>(res[i][2].as<CORBA::ULong>(0));
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_ls_by_type(const char *dom_id, 
			const char * login, const char * pfix, udot_type ut,
			udot_info_list_out uis) std_try {
		
		if( !dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		Result res(NonTransaction(*pg).Exec(
			"SELECT id_conf,val,type FROM udot_ls_by_type("
			+ Quote(dom_id, false)+','
			+ Quote(lower(login), false)+','
			+ Quote(lower(pfix), false)+','
			+ Quote(ToString( static_cast<CORBA::ULong>(ut) )) + ')'));

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		Result::size_type s = res.size();
		uis = new udot_info_list(static_cast<CORBA::ULong>(s));
		uis->length(static_cast<CORBA::ULong>(s));
		for( Result::size_type i=0; i<s; ++i ) {
			uis[i].id_conf = CORBA::string_dup(
				res[i][0].is_null() ? "" : res[i][0].c_str());
			uis[i].val = CORBA::string_dup(
				res[i][1].is_null() ? "" : res[i][1].c_str());
			uis[i].type = static_cast<udot_type>(res[i][2].as<CORBA::ULong>(0));
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_rm(const char * udot_id) std_try {
		if( ! udot_id ) throw ::vq::null_error(__FILE__, __LINE__);
	
		NonTransaction(*pg).Exec("SELECT UDOT_RM("+Quote(udot_id, false)+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_rm_by_type(const char * dom_id,
			const char * login, const char * ext, 
			udot_type ut) std_try {

		if( ! dom_id || ! login || ! ext ) 
				throw ::vq::null_error(__FILE__, __LINE__);
	
		NonTransaction(*pg).Exec("SELECT USER_RM_BY_TYPE("
			+ Quote(dom_id, false)+','
			+ Quote(lower(login), false)+','
			+ Quote(lower(ext), false) + ','
			+ Quote(ToString( static_cast<CORBA::ULong>(ut) )) + ')');

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_get( udot_info & ui ) std_try {
		Result res(NonTransaction(*pg).Exec(
			"SELECT TYPE,VAL FROM UDOT_GET("
			+ Quote(static_cast<const char *>(ui.id_conf), false) + ')'));
	
		if(res.empty())
				return lr(::vq::ivq::err_noent, "");
		
		ui.type = static_cast<udot_type>(res[0][0].as< CORBA::ULong >(0));
		ui.val = res[0][1].is_null() 
				? static_cast<const char *>("") : res[0][1].c_str();
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_rep( 
			const udot_info & ui ) std_try {
	
		Result res(NonTransaction(*pg).Exec(
				"SELECT UDOT_REP("
				+Quote(static_cast<const char *>(ui.id_conf), false)+','
				+Quote(ToString( static_cast<CORBA::ULong>(ui.type) ), false)+','
				+Quote(static_cast<const char *>(ui.val), false)+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "UDOT_REP");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_noent, "")
						: lr(::vq::ivq::err_func_res, "UDOT_REP");
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_type_has(
			const char * dom_id, const char *login, const char *pfix,
			udot_type ut ) std_try {

		if( ! dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		Result res(NonTransaction(*pg).Exec(
			"SELECT UDOT_TYPE_HAS("
			+ Quote(dom_id) + ','
			+ Quote(lower(login)) + ','
			+ Quote(lower(pfix)) + ','
			+ Quote(ToString( static_cast<CORBA::ULong>(ut) )) + ')'));
	
		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "UDOT_TYPE_HAS");
		}

		const char *val = res[0][0].c_str();
		if( *val && '\0' == *(val+1) ) {
				switch(*val) {
				case '0': return lr(::vq::ivq::err_no, "");
				case '1': return lr(::vq::ivq::err_noent, "");
				}
		}
		return lr(::vq::ivq::err_func_res, "UDOT_TYPE_HAS");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::udot_type_cnt(
			const char * dom_id, const char *login, const char *pfix,
			udot_type ut, CORBA::ULong &cnt ) std_try {

		if( ! dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		Result res(NonTransaction(*pg).Exec(
			"SELECT UDOT_TYPE_CNT("
			+ Quote(dom_id) + ','
			+ Quote(lower(login)) + ','
			+ Quote(lower(pfix)) + ','
			+ Quote(ToString( static_cast<CORBA::ULong>(ut) )) + ')'));
	
		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "UDOT_TYPE_HAS");
		}

		cnt = res[0][0].as< CORBA::ULong > (0);
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::qt_user_def_set( const char *dom_id,
			quota_type bytes_max, quota_type files_max ) std_try {

		if( ! dom_id )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		NonTransaction(*pg).Exec("SELECT QT_USER_DEF_SET("
			+Quote(dom_id)+','
			+Quote(ToString(bytes_max))+','
			+Quote(ToString(files_max))+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Gets default quota for new users and sends it via cso
	 * \param dom domain
	 */
	cpgsqlauth::error * cpgsqlauth::qt_user_def_get( const char * dom_id,
			quota_type_out bytes_max, quota_type_out files_max ) std_try {

		if( ! dom_id )
				throw ::vq::null_error(__FILE__, __LINE__);

		Result res(NonTransaction(*pg).Exec(
			"SELECT * FROM QT_USER_DEF_GET("+Quote(dom_id)
			+") AS (qt_user_bytes_def int, qt_user_files_def int)"));
	
		if( res.empty() )
				return lr(::vq::ivq::err_func_res, "QT_USER_DEF_GET");

		if( res[0][0].is_null() || res[0][1].is_null() )
				return lr(::vq::ivq::err_noent, "");

		res[0][0].to<quota_type>(bytes_max);
		res[0][1].to<quota_type>(files_max);

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Gets limits for specified user
	 */
	cpgsqlauth::error * cpgsqlauth::qt_user_get( const char * dom_id, 
			const char * login,
			quota_type_out bytes_max, quota_type_out files_max ) std_try {
		if( ! dom_id || ! login )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
			"SELECT * from QT_USER_GET("+Quote(dom_id)+','+Quote(lower(login))
			+") as (qt_bytes_max int, qt_files_max int)"));

		if( res.empty() )
				return lr(::vq::ivq::err_func_res, "QT_USER_GET");

		if( res[0][0].is_null() || res[0][1].is_null() )
				return lr(::vq::ivq::err_noent, "");
		
		res[0][0].to<quota_type>(bytes_max);
		res[0][1].to<quota_type>(files_max);

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Sets limits for specified user
	 */
	cpgsqlauth::error * cpgsqlauth::qt_user_set( const char * dom_id,
			const char * login,
			quota_type bytes_max, quota_type files_max ) std_try {

		if( ! dom_id || ! login )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		NonTransaction(*pg).Exec("SELECT QT_USER_SET("
			+Quote(dom_id)+','+Quote(lower(login))+','
			+Quote(ToString(bytes_max))+','
			+Quote(ToString(files_max))+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::lr_wrap(err_code ec, const char *what,
			const char * file, CORBA::ULong line ) std_try {
		auto_ptr<error> err(new error);
		err->ec = ec;
		err->what = CORBA::string_dup(what); // string_dup not really needed
		err->file = CORBA::string_dup(file);
		err->line = line;
		err->auth = TRUE;
		return err.release();
	} std_catch

} // namespace POA_vq
