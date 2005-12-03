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
#ifndef __CPGSQLAUTH_HPP
#define __CPGSQLAUTH_HPP

#include "../auth.hpp"
#include "pgsqlcommon.hpp"
#include "cpgsqlpool.hpp"

#include <memory>

namespace POA_vq {

	/**
	 * 
	 */
	class cpgsqlauth : public iauth, 
		public PortableServer::RefCountServantBase {
			public:
					typedef ::vq::iauth::error error;
					typedef ::vq::iauth::err_code err_code;
					typedef ::vq::iauth::string_list string_list;
					typedef ::vq::iauth::string_list_out string_list_out;
					typedef ::vq::iauth::user_conf_info user_conf_info;
					typedef ::vq::iauth::user_conf_info_list user_conf_info_list;
					typedef ::vq::iauth::user_conf_info_list_out user_conf_info_list_out;
					typedef ::vq::iauth::user_conf_type_out user_conf_type_out;
					typedef ::vq::iauth::user_conf_type user_conf_type;
					typedef ::vq::iauth::user_info user_info;
					typedef ::vq::iauth::user_info_list user_info_list;
					typedef ::vq::iauth::user_info_list_out user_info_list_out;
					typedef ::vq::iauth::quota_type quota_type;
					typedef ::vq::iauth::quota_type_out quota_type_out;
					typedef ::vq::iauth::size_type size_type;
					typedef ::vq::iauth::size_type_out size_type_out;
					typedef ::vq::iauth::email_banned email_banned;
					typedef ::vq::iauth::email_banned_list_out email_banned_list_out;
					typedef ::vq::iauth::email_banned_list email_banned_list;
					typedef ::vq::iauth::domain_info_list_out domain_info_list_out;
					typedef ::vq::iauth::domain_info_list domain_info_list;
					typedef ::vq::iauth::id_type id_type;

					cpgsqlauth( const std::string &pginfo, size_type pgs );
					virtual ~cpgsqlauth();
					
				    virtual error * dom_add( const char* dom, 
							id_type & dom_id );
					virtual error * dom_rm( id_type dom );
				    virtual error * dom_id( const char* dom, 
							id_type & dom_id );
    				virtual error * dom_name( id_type dom_id, 
							CORBA::String_out domain );
					virtual error * dom_ls( domain_info_list_out dis );

					virtual error * da_add( id_type dom_id, const char* rea );
    				virtual error * da_rm( const char* rea );
    				virtual error * da_ls_by_dom( id_type dom_id, 
							string_list_out rea );

					virtual error * dip_add( id_type dom_id, const char* rea );
    				virtual error * dip_rm( const char* rea );
    				virtual error * dip_ls_by_dom( id_type dom_id, 
							string_list_out rea );

				    virtual error * user_add( const user_info & ai,
							CORBA::Boolean is_banned );
				    virtual error * user_rep( const user_info & ai,
							CORBA::Boolean password, CORBA::Boolean dir );
				    virtual error * user_rm( id_type dom_id, const char* user );
				    virtual error * user_pass( id_type dom_id, const char* user, 
							const char* pass );
					virtual error * user_get( user_info& ai );
					virtual error * user_ex( id_type dom_id, const char *user);
					virtual error * user_ls_by_dom( id_type dom_id, size_type start, size_type cnt, user_info_list_out uis);
    				virtual error * user_cnt_by_dom( id_type dom_id, size_type& cnt);
					
				    virtual error * eb_add( const char * re_domain, 
							const char * re_login );
    				virtual error * eb_rm( const char * re_domain,
							const char * re_login );
    				virtual error * eb_ls( email_banned_list_out ebs );
    
					virtual error* qt_user_get( id_type dom_id, 
							const char* user, quota_type_out bytes_max, 
							quota_type_out files_max );
				    virtual error* qt_user_set( id_type dom_id, const char* user, 
							quota_type bytes_max, quota_type files_max );
				    virtual error* qt_user_def_set( id_type dom_id, 
							quota_type bytes_max, quota_type files_max );
				    virtual error* qt_user_def_get( id_type dom_id, 
							quota_type_out bytes_max, quota_type_out files_max );
				    
					virtual error* user_conf_add( id_type dom_id, const char* user, 
							const char* pfix, user_conf_info &ui );
				    virtual error* user_conf_rm_by_type( id_type dom_id, 
							const char* user, const char *pfix, user_conf_type ut );
				    virtual error* user_conf_rm( id_type id );
				    virtual error* user_conf_ls( id_type dom_id,
							const char* user, const char* pfix, 
							user_conf_info_list_out uis );
				    virtual error* user_conf_ls_by_type( id_type dom_id, 
							const char* user, const char* pfix, 
							user_conf_type ut, user_conf_info_list_out uis );
				    virtual error* user_conf_rep( const user_conf_info& ui );
				    virtual error* user_conf_get( user_conf_info& ui );
			    	virtual error* user_conf_type_has( id_type dom_id, 
							const char* user, const char* pfix, user_conf_type ut );
			    	virtual error* user_conf_type_cnt( id_type dom_id, 
							const char* user, const char* pfix, 
							user_conf_type ut, size_type_out cnt );

			protected:
					/// pgsql
					cpgsqlpool pool;
					
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

				/**
				 * \defgroup da_dip functions used by da_... and dip_...
				 */
				//@{
					virtual error * da_dip_add( const std::string & dom_id, 
							const char* val, const std::string & func );
    				virtual error * da_dip_rm( const char* rea,
							const std::string & func );
    				virtual error * da_dip_ls_by_dom( const std::string & dom_id, 
							string_list_out rea, 
							const std::string &, const std::string & func );
				//@}

					virtual error * user_eb_rm( const std::string & dom_id, 
							const std::string & val, const std::string & func );

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

#endif // ifndef __CPGSQLAUTH_HPP
