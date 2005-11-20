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

#include <boost/lexical_cast.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdexcept>
#include <iostream>

namespace POA_vq {
	
	using namespace std;
	using namespace pqxx;
	using namespace text;
	using sys::str2tb;

	/**
	 * \param pginfo Connection configuration
	 */
	cpgsqlauth::cpgsqlauth( const std::string &pginfo, size_type pgs ) 
			: pool(pginfo, pgs) std_try {
	} std_catch;
	
	/**
	 *
	 */
	cpgsqlauth::~cpgsqlauth() std_try {
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_add( const char * _dom,
			id_type & dom_id ) std_try {
		if( ! _dom ) throw ::vq::null_error(__FILE__, __LINE__);

		string dom(lower(_dom));

		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT DOM_ADD('"+sqlesc(dom)+"')"));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "DOM_ADD");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				return ( '1' == *(val+1) ) 
						? lr(::vq::ivq::err_exists, dom) 
						: lr(::vq::ivq::err_func_res, "DOM_ADD");
						
		}
		dom_id = res[0][0].as<id_type>(0);
		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_id( const char * dom,
			id_type & dom_id ) std_try {
		if( !dom ) throw ::vq::null_error(__FILE__, __LINE__);
		
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT DOM_ID('"+sqlesc(lower(dom))+"')"));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_noent, 
					boost::lexical_cast<std::string>(dom_id));
		}

		dom_id = res[0][0].as<id_type>(static_cast<id_type>(-1));
		if( static_cast<id_type>(-1) == dom_id )
			return lr(::vq::ivq::err_noent, "");

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_name( id_type dom_id,
			CORBA::String_out domain ) std_try {
		domain = CORBA::string_dup("");
		
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT domain FROM vq_view_DOM_NAME WHERE id_domain="
			+to_string(dom_id) ));

		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_noent, 
					boost::lexical_cast<std::string>(dom_id));
		}

		domain = res[0][0].c_str();
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::dom_rm( id_type dom_id ) std_try {
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT DOM_RM("+to_string(dom_id)+')'));

		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, "DOM_RM");

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dom_ls( domain_info_list_out dis ) std_try {
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT id_domain,domain FROM vq_view_dom_ls"));

		dis = new domain_info_list;

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		result::size_type s = res.size();
		dis->length(static_cast<CORBA::ULong>(s));
		for( result::size_type i=0; i<s; ++i ) {
			dis[i].id_domain = res[i][0].as<id_type>(id_type());
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
		
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
				"SELECT USER_ADD("
				+to_string(ai.id_domain)+",'"
				+sqlesc(lower(static_cast<const char *>(ai.login)))+"','"
				+sqlesc(static_cast<const char *>(ai.pass))+"','"
				+sqlesc(static_cast<const char *>(ai.dir))+"','"
				+to_string(ai.flags)+"',"
				+(is_banned ? "'t'" : "'f'")+"::boolean)"));
	
		if(res.empty() || res[0][0].is_null() ) {
				return lr(::vq::ivq::err_func_res, "USER_ADD");
		}

		const char *val = res[0][0].c_str();
		if( '-' == *val ) {
				switch( *(val+1) ) {
				case '3':
						return lr(::vq::ivq::err_noent, 
							boost::lexical_cast<std::string>(ai.id_domain));
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
	cpgsqlauth::error * cpgsqlauth::da_add( id_type dom_id,
			const char * ali ) std_try {
		return da_dip_add(
			boost::lexical_cast<std::string>(dom_id), ali, "DA_ADD");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dip_add( id_type dom_id,
			const char * ali ) std_try {
		in_addr addr;
		in6_addr addr6;
		if( inet_aton(ali ? ali : "", &addr) != 1
			&& inet_pton(AF_INET6, ali ? ali : "", &addr6 ) != 1 )
				return lr(::vq::ivq::err_dom_inv, ali);
		return da_dip_add(
			boost::lexical_cast<std::string>(dom_id), ali, "DIP_ADD");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::da_dip_add( const std::string & dom_id, 
			const char* rea, const std::string & func ) std_try {
		if( ! rea )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
				"SELECT "+func+"('"+sqlesc(dom_id)+"','"+sqlesc(rea)+"')"));

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
		
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
				"SELECT "+func+"('"+sqlesc(static_cast<const char *>(rea))+"')"));

		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, func);

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::da_ls_by_dom( id_type dom_id, 
			string_list_out reas ) std_try {

		return da_dip_ls_by_dom( boost::lexical_cast<std::string>(dom_id), 
			reas, "alias", "vq_view_DA_LS");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::dip_ls_by_dom( id_type dom_id, 
			string_list_out reas ) std_try {

		return da_dip_ls_by_dom( boost::lexical_cast<std::string>(dom_id), 
			reas, "ip", "vq_view_DIP_LS");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::da_dip_ls_by_dom( 
			const std::string & dom_id, string_list_out reas, 
			const std::string & field, const std::string & view ) std_try {

		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT "+field+" FROM "+view+" WHERE id_domain="+to_string(dom_id) ));

		reas = new string_list;

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		result::size_type s = res.size();
		reas->length(static_cast<CORBA::ULong>(s));
		for( result::size_type i=0; i<s; ++i ) {
			reas[i] = CORBA::string_dup(
				res[i][0].is_null() ? "" : res[i][0].c_str());
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 * 
	 */
	cpgsqlauth::error * cpgsqlauth::user_pass( id_type dom_id, 
			const char *login, const char *_pass ) std_try {
		if( ! login || ! _pass )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
				"SELECT USER_PASS("+to_string(dom_id)+",'"
				+sqlesc(lower(login))+"','"+sqlesc(_pass)+"')"));

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
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_cnt_by_dom(
			id_type dom_id, CORBA::ULong &cnt ) std_try {

		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT count FROM vq_view_USER_CNT_BY_DOM"
			" WHERE id_domain="+ to_string(dom_id) ));
	
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
	cpgsqlauth::error * cpgsqlauth::user_ls_by_dom( id_type dom_id, 
			size_type start, size_type cnt, user_info_list_out uis ) std_try {
		string qr("SELECT pass,dir,flags,login FROM vq_view_user_get"
			" WHERE id_domain="+to_string(dom_id)+" ORDER BY login");
		if( cnt ) {
				qr += " LIMIT "+to_string(cnt);
		}
		if( start ) {
				qr +=  " OFFSET "+to_string(start);
		}
			
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(qr));

		uis = new user_info_list;

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		result::size_type s = res.size();
		uis->length(static_cast<CORBA::ULong>(s));
		for( result::size_type i=0; i<s; ++i ) {
			uis[i].pass = res[i][0].c_str();
			uis[i].dir = res[i][1].c_str();
			uis[i].flags = res[i][2].as< ::vq::iauth::uif_type >(0);
			uis[i].login = res[i][3].c_str();
			uis[i].id_domain = dom_id;
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::user_rm( id_type dom_id, 
			const char *login) std_try {
		if( !login )
				throw ::vq::null_error(__FILE__, __LINE__);
		return user_eb_rm(boost::lexical_cast<std::string>(dom_id),
			lower(login), "USER_RM");
	} std_catch

	/**
	*/
	cpgsqlauth::error * cpgsqlauth::user_ex( id_type dom_id,
			const char * login ) std_try {
		if( ! login ) throw ::vq::null_error(__FILE__, __LINE__);

		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT EXISTS (SELECT * FROM vq_view_USER_EX "
			"WHERE id_domain="+to_string(dom_id)
			+" AND login='"+sqlesc(lower(login))+"')" ));

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
	cpgsqlauth::error * cpgsqlauth::user_eb_rm( const std::string & dom_id,
				const std::string & login, const std::string & func ) std_try {
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
				"SELECT "+func+"('"+sqlesc(dom_id)+"','"+sqlesc(login)+"')"));

		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, func);

		return lr(::vq::ivq::err_no, "");
	} std_catch


	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::eb_add( const char * re_domain,
			const char * re_login ) std_try {
		if( !re_domain )
				throw ::vq::null_error(__FILE__, __LINE__);
		return da_dip_add(re_domain, re_login, "EB_ADD");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::eb_rm( const char * re_domain,
			const char * re_login ) std_try {
		if( !re_domain || !re_login )
				throw ::vq::null_error(__FILE__, __LINE__);
		return user_eb_rm( re_domain, re_login, "EB_RM" );
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::eb_ls( email_banned_list_out ebs ) std_try {
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT re_domain,re_login FROM vq_view_eb_ls"));
		
		ebs = new email_banned_list;
		
		if(res.empty()) return lr(::vq::ivq::err_no, "");

		result::size_type s = res.size();
		ebs->length(static_cast<CORBA::ULong>(s));
		for( result::size_type i=0; i<s; ++i ) {
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
		
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT pass,dir,flags FROM vq_view_user_get"
			" WHERE id_domain="+to_string(ai.id_domain)+" AND login='"
			+sqlesc(lower(static_cast<const char *>(ai.login))) + '\''));
		
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
	cpgsqlauth::error * cpgsqlauth::user_conf_add( id_type dom_id,
			const char *_login, const char * pfix, 
			user_conf_info &ui ) std_try {

		if( ! _login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
		std::string login(lower(_login));	
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT USER_CONF_ADD("
			+ to_string(dom_id)+",'"+sqlesc(login)+"','"
			+ sqlesc(lower(pfix))+"','"
			+ sqlesc(to_string(static_cast<CORBA::ULong>(ui.type)))+"','"
			+ sqlesc(static_cast<const char *>(ui.val)) + "')" ) );
	
		if(res.empty() || res[0][0].is_null()) {
				return lr(::vq::ivq::err_func_res, "USER_CONF_ADD");
		}
		ui.id_conf = res[0][0].as<id_type>(id_type());
		if( static_cast< id_type >(-1) == ui.id_conf )
				return lr(::vq::ivq::err_noent, 
					boost::lexical_cast<std::string>(ui.id_conf));
		if( static_cast< id_type >(-2) == ui.id_conf )
				return lr(::vq::ivq::err_noent, login);
		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_ls(id_type dom_id, 
			const char * login, const char * pfix, 
			user_conf_info_list_out uis) std_try {
		
		if( ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT id_conf,val,type FROM vq_view_user_conf_ls "
			"WHERE id_domain="+ to_string(dom_id)
			+" AND login='"+sqlesc(lower(login))+"' AND ext='"
			+ sqlesc(lower(pfix))+'\'' ));

		uis = new user_conf_info_list;

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		result::size_type s = res.size();
		uis->length(static_cast<CORBA::ULong>(s));
		for( result::size_type i=0; i<s; ++i ) {
			uis[i].id_conf = res[i][0].as<id_type>(id_type());
			uis[i].val = res[i][1].is_null() 
				? static_cast<const char *>("") : res[i][1].c_str();
			uis[i].type = static_cast<user_conf_type>(res[i][2].as<CORBA::ULong>(0));
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_ls_by_type( id_type dom_id, 
			const char * login, const char * pfix, user_conf_type ut,
			user_conf_info_list_out uis) std_try {
		
		if( ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);

		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT id_conf,val,type FROM vq_view_user_conf_ls"
			" WHERE id_domain="+ to_string(dom_id)
			+" AND login='"+ sqlesc(lower(login))+"' AND ext='"
			+ sqlesc(lower(pfix))+"' AND type='"
			+ sqlesc(to_string( static_cast<CORBA::ULong>(ut))) + '\'' ));

		uis = new user_conf_info_list;

		if(res.empty()) return lr(::vq::ivq::err_no, "");

		result::size_type s = res.size();
		uis->length(static_cast<CORBA::ULong>(s));
		for( result::size_type i=0; i<s; ++i ) {
			uis[i].id_conf = res[i][0].as<id_type>(id_type());
			uis[i].val = CORBA::string_dup(
				res[i][1].is_null() ? "" : res[i][1].c_str());
			uis[i].type = static_cast<user_conf_type>(res[i][2].as<CORBA::ULong>(0));
		}
		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_rm(id_type user_conf_id) std_try {
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT USER_CONF_RM("+to_string(user_conf_id)+')'));

		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, "USER_CONF_RM");

		return lr(::vq::ivq::err_no, "");
	} std_catch
	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_rm_by_type(id_type dom_id,
			const char * login, const char * ext, 
			user_conf_type ut) std_try {

		if( ! login || ! ext ) 
				throw ::vq::null_error(__FILE__, __LINE__);
	
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec("SELECT USER_CONF_RM_BY_TYPE("
			+ to_string(dom_id)+",'"
			+ sqlesc(lower(login))+"','"
			+ sqlesc(lower(ext))+"','"
			+ sqlesc(to_string( static_cast<CORBA::ULong>(ut) )) + "')"));

		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, "USER_CONF_RM_BY_TYPE");

		return lr(::vq::ivq::err_no, "");
	} std_catch

	/**
	 *
	 */
	cpgsqlauth::error * cpgsqlauth::user_conf_get( user_conf_info & ui ) std_try {
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT TYPE,VAL FROM vq_view_USER_CONF_GET WHERE id_conf="
			+to_string(ui.id_conf) ));
	
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
	
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
				"SELECT USER_CONF_REP("+to_string(ui.id_conf)+','
				+to_string( static_cast<CORBA::ULong>(ui.type) )+",'"
				+sqlesc(static_cast<const char *>(ui.val)) +"')"));

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
			id_type dom_id, const char *login, const char *pfix,
			user_conf_type ut ) std_try {

		if( ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT EXISTS (SELECT * FROM vq_view_user_conf_type_has WHERE "
			"id_domain="+to_string(dom_id)
			+" AND login='"+sqlesc(lower(login))
			+"' AND ext='"+sqlesc(lower(pfix))
			+"' AND type='"+sqlesc(to_string( static_cast<CORBA::ULong>(ut) ))
			+"')" ));
	
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
			id_type dom_id, const char *login, const char *pfix,
			user_conf_type ut, CORBA::ULong &cnt ) std_try {

		if( ! login || ! pfix )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT count FROM vq_view_USER_CONF_TYPE_CNT"
			" WHERE id_domain="+ to_string(dom_id)
			+ " AND login='"+ sqlesc(lower(login))
			+ "' AND ext='"+ sqlesc(lower(pfix))
			+ "' AND type='"+ sqlesc(to_string( static_cast<CORBA::ULong>(ut) ))
			+ '\''));
	
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
	cpgsqlauth::error * cpgsqlauth::qt_user_def_set( id_type dom_id,
			quota_type bytes_max, quota_type files_max ) std_try {

		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec("SELECT QT_USER_DEF_SET("
			+to_string(dom_id)+','
			+to_string(bytes_max)+','
			+to_string(files_max)+')'));

		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, "QT_USER_DEF_SET");

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Gets default quota for new users and sends it via cso
	 * \param dom domain
	 */
	cpgsqlauth::error * cpgsqlauth::qt_user_def_get( id_type dom_id,
			quota_type_out bytes_max, quota_type_out files_max ) std_try {

		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT qt_user_bytes_def,qt_user_files_def "
			"FROM vq_view_QT_USER_DEF_GET WHERE id_domain="+to_string(dom_id) ));
	
		if( res.empty() )
				return lr(::vq::ivq::err_noent, "");

		res[0][0].to<quota_type>(bytes_max);
		res[0][1].to<quota_type>(files_max);

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Gets limits for specified user
	 */
	cpgsqlauth::error * cpgsqlauth::qt_user_get( id_type dom_id, 
			const char * login,
			quota_type_out bytes_max, quota_type_out files_max ) std_try {
		if( ! login )
				throw ::vq::null_error(__FILE__, __LINE__);
		
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec(
			"SELECT qt_bytes_max,qt_files_max from vq_view_QT_USER_GET"
			" WHERE id_domain="+to_string(dom_id)
			+" AND login='"+ sqlesc(lower(login)) + '\'' ));

		if( res.empty() )
				return lr(::vq::ivq::err_noent, "");

		res[0][0].to<quota_type>(bytes_max);
		res[0][1].to<quota_type>(files_max);

		return lr(::vq::ivq::err_no, "");
	} std_catch
	
	/**
	 * Sets limits for specified user
	 */
	cpgsqlauth::error * cpgsqlauth::qt_user_set( id_type dom_id,
			const char * login,
			quota_type bytes_max, quota_type files_max ) std_try {

		if( ! login )
				throw ::vq::null_error(__FILE__, __LINE__);
	
		cpgsqlpool::value_ptr pg(pool.get());
		result res(nontransaction(*pg.get()).exec("SELECT QT_USER_SET("
			+to_string(dom_id)+",'"+sqlesc(lower(login))+"',"
			+to_string(bytes_max)+','
			+to_string(files_max)+')'));

		if( res.empty() || res[0][0].is_null() 
			|| strcmp(res[0][0].c_str(), "0") )
				return lr(::vq::ivq::err_func_res, "QT_USER_SET");

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
		err->auth = 1;
		return err.release();
	} std_catch

} // namespace POA_vq
