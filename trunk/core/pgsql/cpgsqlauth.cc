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
#include "cpgsqlauth.hpp"
#include "pgsqlcommon.hpp"

#include <text.hpp>
#include <util.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdexcept>

namespace POA_vq {
	
	using namespace std;
	using namespace pqxx;
	using namespace text;
	using sys::str2tb;

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
			"SELECT domain FROM vq_view_DOM_NAME WHERE id_domain="
			+Quote(dom_id, false) ));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_noent, dom_id);
		}

		domain = res[0][0].c_str();
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
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dom_ls( domain_info_list_out dis ) std_try {
		Result res(NonTransaction(*pg).Exec(
			"SELECT id_domain,domain FROM vq_view_dom_ls"));
		if(res.empty()) return lr(::vq::ivq::err_no, "");

		Result::size_type s = res.size();
		dis = new domain_info_list(static_cast<CORBA::ULong>(s));
		dis->length(static_cast<CORBA::ULong>(s));
		for( Result::size_type i=0; i<s; ++i ) {
			dis[i].id_domain = CORBA::string_dup(
				res[i][0].is_null() ? "" : res[i][0].c_str());
			dis[i].domain = CORBA::string_dup(
				res[i][1].is_null() ? "" : res[i][1].c_str());
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::user_add( const user_info & ai, 
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
	cpgsqlauth::error * cpgsqlauth::da_add( const char * dom_id,
			const char * ali ) std_try {
		return da_dip_add(dom_id, ali, "DA_ADD");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dip_add( const char * dom_id,
			const char * ali ) std_try {
		in_addr addr;
		in6_addr addr6;
		if( inet_aton(ali ? ali : "", &addr) != 1
			&& inet_pton(AF_INET6, ali ? ali : "", &addr6 ) != 1 )
				return lr(::vq::ivq::err_dom_inv, ali);
		return da_dip_add(dom_id, ali, "DIP_ADD");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::da_dip_add( const char* dom_id, 
			const char* rea, const std::string & func ) std_try {
		if( ! dom_id || ! rea )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
				"SELECT "+func+"("
				+Quote(static_cast<const char *>(dom_id))+','
				+Quote(static_cast<const char *>(rea))+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, func);
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_exists, "")
						: lr(::vq::ivq::err_func_res, func);
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::da_rm( const char* rea ) std_try {
		return da_dip_rm( rea, "DA_RM" );
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dip_rm( const char* rea ) std_try {
		return da_dip_rm( rea, "DIP_RM" );
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::da_dip_rm( const char* rea,
				const std::string & func ) std_try {
		if( ! rea )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		NonTransaction(*pg).Exec(
				"SELECT "+func+"("+Quote(static_cast<const char *>(rea))+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::da_ls_by_dom( const char* dom_id, 
			string_list_out reas ) std_try {

		return da_dip_ls_by_dom( dom_id, reas, "alias", "vq_view_DA_LS");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dip_ls_by_dom( const char* dom_id, 
			string_list_out reas ) std_try {

		return da_dip_ls_by_dom( dom_id, reas, "ip", "vq_view_DIP_LS");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::da_dip_ls_by_dom( const char* dom_id, 
			string_list_out reas, 
			const std::string & field, const std::string & view ) std_try {

		Result res(NonTransaction(*pg).Exec(
			"SELECT "+field+" FROM "+view+" WHERE id_domain="
			+Quote(static_cast<const char *>(dom_id)) ));
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
			"SELECT EXISTS (SELECT * FROM vq_view_USER_EX "
			"WHERE id_domain="+Quote(dom_id, false)
			+" AND login="+Quote(lower(login), false)+')' ));

		if(res.empty()) {
				return lr(::vq::ivq::err_noent, "");
		}

		const char * ex = res[0][0].c_str();
		return lr(ex && 't' == *ex 
			? ::vq::ivq::err_no : ::vq::ivq::err_noent, "" );
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
			"SELECT re_domain,re_login FROM vq_view_eb_ls"));
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
	cpgsqlauth::error * cpgsqlauth::user_get( user_info & ai ) std_try {
		if( ! ai.id_domain || ! ai.login )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
			"SELECT pass,dir,flags FROM vq_view_user_get"
			" WHERE id_domain="+Quote(static_cast<const char *>(ai.id_domain))
			+" AND login="+Quote(lower(static_cast<const char *>(ai.login))) ));
		
		if( res.empty() ) 
				return lr(::vq::ivq::err_noent, "");
		if( res.size() != 1 )
				return lr(::vq::ivq::err_func_res, "");

		ai.pass = res[0][0].c_str();
		ai.dir = res[0][1].c_str();
		ai.flags = res[0][2].as< ::vq::iauth::uif_type >(0);
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_add( const char * dom_id,
			const char *login, const char * pfix, 
			user_conf_info &ui ) std_try {

		if( ! dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		Result res(NonTransaction(*pg).Exec(
			"SELECT USER_CONF_ADD("
			+ Quote(dom_id, false)+','
			+ Quote(lower(login), false)+','
			+ Quote(lower(pfix), false)+','
			+ Quote(ToString( static_cast<CORBA::ULong>(ui.type) ))+','
			+ Quote(static_cast<const char *>(ui.val), false)+')'));
	
		if(res.empty() || res[0][0].is_null()) {
				return lr(::vq::ivq::err_func_res, "USER_CONF_ADD");
		}
		ui.id_conf = res[0][0].c_str();
		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_ls(const char *dom_id, 
			const char * login, const char * pfix, 
			user_conf_info_list_out uis) std_try {
		
		if( !dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		Result res(NonTransaction(*pg).Exec(
			"SELECT id_conf,val,type FROM vq_view_user_conf_ls "
			"WHERE id_domain="+ Quote(dom_id, false)
			+" AND login="+ Quote(lower(login), false)
			+" AND ext="+ Quote(lower(pfix), false) ));

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		Result::size_type s = res.size();
		uis = new user_conf_info_list(static_cast<CORBA::ULong>(s));
		uis->length(static_cast<CORBA::ULong>(s));
		for( Result::size_type i=0; i<s; ++i ) {
			uis[i].id_conf = res[i][0].is_null() 
				? static_cast<const char *>("") : res[i][0].c_str();
			uis[i].val = res[i][1].is_null() 
				? static_cast<const char *>("") : res[i][1].c_str();
			uis[i].type = static_cast<user_conf_type>(res[i][2].as<CORBA::ULong>(0));
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_ls_by_type(const char *dom_id, 
			const char * login, const char * pfix, user_conf_type ut,
			user_conf_info_list_out uis) std_try {
		
		if( !dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		Result res(NonTransaction(*pg).Exec(
			"SELECT id_conf,val,type FROM vq_view_user_conf_ls"
			" WHERE id_domain="+ Quote(dom_id, false)
			+" AND login="+ Quote(lower(login), false)
			+" AND ext="+ Quote(lower(pfix), false)
			+" AND type="+Quote(ToString( static_cast<CORBA::ULong>(ut) )) ));

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		Result::size_type s = res.size();
		uis = new user_conf_info_list(static_cast<CORBA::ULong>(s));
		uis->length(static_cast<CORBA::ULong>(s));
		for( Result::size_type i=0; i<s; ++i ) {
			uis[i].id_conf = CORBA::string_dup(
				res[i][0].is_null() ? "" : res[i][0].c_str());
			uis[i].val = CORBA::string_dup(
				res[i][1].is_null() ? "" : res[i][1].c_str());
			uis[i].type = static_cast<user_conf_type>(res[i][2].as<CORBA::ULong>(0));
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_rm(const char * user_conf_id) std_try {
		if( ! user_conf_id ) throw ::vq::null_error(__FILE__, __LINE__);
	
		NonTransaction(*pg).Exec("SELECT USER_CONF_RM("+Quote(user_conf_id, false)+')');

		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_rm_by_type(const char * dom_id,
			const char * login, const char * ext, 
			user_conf_type ut) std_try {

		if( ! dom_id || ! login || ! ext ) 
				throw ::vq::null_error(__FILE__, __LINE__);
	
		NonTransaction(*pg).Exec("SELECT USER_CONF_RM_BY_TYPE("
			+ Quote(dom_id, false)+','
			+ Quote(lower(login), false)+','
			+ Quote(lower(ext), false) + ','
			+ Quote(ToString( static_cast<CORBA::ULong>(ut) )) + ')');

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_get( user_conf_info & ui ) std_try {
		Result res(NonTransaction(*pg).Exec(
			"SELECT TYPE,VAL FROM vq_view_USER_CONF_GET WHERE id_conf="
			+ Quote(static_cast<const char *>(ui.id_conf), false) ));
	
		if(res.empty())
				return lr(::vq::ivq::err_noent, "");
		
		ui.type = static_cast<user_conf_type>(res[0][0].as< CORBA::ULong >(0));
		ui.val = res[0][1].is_null() 
				? static_cast<const char *>("") : res[0][1].c_str();
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_rep( 
			const user_conf_info & ui ) std_try {
	
		Result res(NonTransaction(*pg).Exec(
				"SELECT USER_CONF_REP("
				+Quote(static_cast<const char *>(ui.id_conf), false)+','
				+Quote(ToString( static_cast<CORBA::ULong>(ui.type) ), false)+','
				+Quote(static_cast<const char *>(ui.val), false)+')'));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "USER_CONF_REP");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_noent, "")
						: lr(::vq::ivq::err_func_res, "USER_CONF_REP");
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_type_has(
			const char * dom_id, const char *login, const char *pfix,
			user_conf_type ut ) std_try {

		if( ! dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		Result res(NonTransaction(*pg).Exec(
			"SELECT EXISTS (SELECT * FROM vq_view_user_conf_type_has WHERE "
			"id_domain="+Quote(dom_id)
			+" AND login="+Quote(lower(login))
			+" AND ext="+Quote(lower(pfix))
			+" AND type="+Quote(ToString( static_cast<CORBA::ULong>(ut) ))+')' ));
	
		if(res.empty()) {
				return lr(::vq::ivq::err_noent, "");
		}

		const char * ex = res[0][0].c_str();
		return lr( ex && 't' == *ex 
			? ::vq::ivq::err_no : ::vq::ivq::err_noent, "" );
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_type_cnt(
			const char * dom_id, const char *login, const char *pfix,
			user_conf_type ut, CORBA::ULong &cnt ) std_try {

		if( ! dom_id || ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		Result res(NonTransaction(*pg).Exec(
			"SELECT count FROM vq_view_USER_CONF_TYPE_CNT"
			" WHERE id_domain="+ Quote(dom_id)
			+ " AND login="+ Quote(lower(login))
			+ " AND ext="+ Quote(lower(pfix))
			+ " AND type="+ Quote(ToString( static_cast<CORBA::ULong>(ut) )) ));
	
		if(res.empty()) {
				cnt = 0;
		} else {
				cnt = res[0][0].as< CORBA::ULong > (0);
		}
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
			"SELECT qt_user_bytes_def,qt_user_files_def "
			"FROM vq_view_QT_USER_DEF_GET WHERE id_domain="+Quote(dom_id) ));
	
		if( res.empty() )
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
			"SELECT qt_bytes_max,qt_files_max from vq_view_QT_USER_GET"
			" WHERE id_domain="+Quote(dom_id)
			+" AND login="+Quote(lower(login)) ));

		if( res.empty() )
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
