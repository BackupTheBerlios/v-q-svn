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
#include "../auth.hpp"
#include "pgsqlcommon.hpp"

#include <memory>

namespace POA_vq {

	/**
	 * 
	 */
	class cpgsqlauth : public iauth {
			public:
					typedef ::vq::iauth::error error;
					typedef ::vq::iauth::err_code err_code;
					typedef ::vq::iauth::string_list string_list;
					typedef ::vq::iauth::string_list_out string_list_out;
					typedef ::vq::iauth::udot_info udot_info;
					typedef ::vq::iauth::udot_info_list udot_info_list;
					typedef ::vq::iauth::udot_info_list_out udot_info_list_out;
					typedef ::vq::iauth::udot_info_out udot_info_out;
					typedef ::vq::iauth::udot_type udot_type;
					typedef ::vq::iauth::auth_info auth_info;
					typedef ::vq::iauth::quota_type quota_type;
					typedef ::vq::iauth::quota_type_out quota_type_out;
					typedef ::vq::iauth::size_type size_type;
					typedef ::vq::iauth::size_type_out size_type_out;
					typedef ::vq::iauth::email_banned email_banned;
					typedef ::vq::iauth::email_banned_list_out email_banned_list_out;
					typedef ::vq::iauth::email_banned_list email_banned_list;

					cpgsqlauth( const char *pginfo );
					virtual ~cpgsqlauth();
					
	
				    virtual error * dom_add( const char* dom, 
							CORBA::String_out dom_id );
					virtual error * dom_rm( const char* dom );
				    virtual error * dom_id( const char* dom, 
							CORBA::String_out dom_id );
    				virtual error * dom_name( const char* dom_id, 
							CORBA::String_out domain );

					virtual error * dra_add( const char* dom_id, const char* rea );
    				virtual error * dra_rm( const char* dom_id, const char* rea );
    				virtual error * dra_rm_by_dom( const char* dom_id );
    				virtual error * dra_ls_by_dom( const char* dom_id, 
							string_list_out rea );

				    virtual error * user_add( const auth_info & ai,
							CORBA::Boolean is_banned );
				    virtual error * user_rm( const char* dom, const char* user );
				    virtual error * user_pass( const char* dom, const char* user, 
							const char* pass );
					virtual error * user_auth( auth_info& ai );
					virtual error * user_ex( const char *dom, const char *user);
					
				    virtual error * eb_add( const char * re_domain, 
							const char * re_login );
    				virtual error * eb_rm( const char * re_domain,
							const char * re_login );
    				virtual error * eb_ls( email_banned_list_out ebs );
    
					virtual error * qt_user_get( const char* dom_id, 
							const char* user_id, 
							quota_type_out bytes_max, quota_type_out files_max );
    				virtual error * qt_user_set( const char* dom_id, 
							const char* user_id, 
							quota_type bytes_max, quota_type files_max );
    				virtual error * qt_user_def_set( const char* dom_id, 
							quota_type bytes_max, quota_type files_max );
    				virtual error * qt_user_def_get( const char* dom_id, 
							quota_type_out bytes_max, quota_type_out files_max );

					/*
				    virtual error udot_add( const char* dom, const char* user, 
							const char* pfix, udot_info_out ui );
				    virtual error udot_rm_by_type( const char* dom, 
							const char* user, udot_type ut );
				    virtual error udot_rm_by_id( const char* dom, 
							const char* user, const char* id );
				    virtual error udot_ls( const char* dom, const char* user, 
							const char* pfix, udot_info_list_out uis );
				    virtual error udot_ls_by_type( const char* dom, 
							const char* user, const char* pfix, udot_type ut, 
							udot_info_list_out uis );
				    virtual error udot_rep( const char* dom, const char* user, 
							const udot_info& ui );
				    virtual error udot_get( const char* dom, const char* user, 
							udot_info& ui );
				    virtual error udot_has( const char* dom, const char* user, 
							const char* pfix, udot_type ut );
				    virtual error udot_type_cnt( const char* dom, 
							const char* user, const char* pfix, 
							udot_type ut, size_type_out cnt );
				    virtual char* err_info();
				    virtual CORBA::Long err_sys();
				    virtual char* err_report();
					*/
	
			protected:
					/// pgsql
					std::auto_ptr<pqxx::Connection> pg;
					/*
					void qt_def_get(const std::string &);
					void qt_def_set(const std::string &);
					*/
				/**
				 * \defgroup err Errors handling
				 */
				/*@{*/
#define lr(ec, what) lr_wrap(ec, what, __FILE__, __LINE__) //!< return lastret
					inline error * lr_wrap(err_code, const std::string &, 
							const char *, CORBA::ULong );
					error * lr_wrap(err_code, const char *, 
							const char *, CORBA::ULong );
				/*@}*/	

	};

	/**
	 *
	 */
	inline cpgsqlauth::error * cpgsqlauth::lr_wrap(err_code ec, 
			const std::string & what, 
			const char *file, CORBA::ULong line) {
		return lr_wrap( ec, what.c_str(), file, line);
	}


} // namespace POA_vq
