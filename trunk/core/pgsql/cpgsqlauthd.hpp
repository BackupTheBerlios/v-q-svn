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
#include "pgsqlcommon.hpp"

namespace POA_vq {

	/**
	 * 
	 */
	class cpgsqlauthd : public cdaemonchild {
			public:
					typedef ::vq::iauth::error error;

					cpgsqlauthd();
					virtual ~cpgsqlauthd();
					
	
				    virtual error dom_add( const char* dom );
				    virtual error dom_ip_add( const char* dom, const char* ip );
				    virtual error dom_ip_rm( const char* dom, const char* ip );
				    virtual error dom_ip_rm_all( const char* dom );
				    virtual error dom_ip_ls( const char* dom, 
							string_list_out ips );
				    virtual error dom_ip_ls_dom( string_list_out doms );
				    virtual error dom_rm( const char* dom );
				    virtual error user_add( const char* dom, const char* user, 
							const char* pass, error errex );
				    virtual error user_rm( const char* dom, const char* user );
				    virtual error user_pass( const char* dom, const char* user, 
							const char* pass );
				    virtual error user_auth( auth_info& ai );
				    virtual error user_ex( const char* dom, const char* user );
				    virtual error qt_get( const char* dom, const char* user, 
							quota_type_out bytes_max, quota_type_out files_max );
				    virtual error qt_set( const char* dom, const char* user, 
							quota_type bytes_max, quota_type files_max );
				    virtual error qt_def_set( const char* dom, 
							quota_type bytes_max, quota_type files_max );
				    virtual error qt_def_get( const char* dom, 
							quota_type_out bytes_max, quota_type_out files_max );
				    virtual error qt_global_def_set( 
							quota_type bytes_max, quota_type files_max );
				    virtual error qt_global_def_get( 
							quota_type_out bytes_max, quota_type_out files_max );
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
				
	
			protected:
					void setup();
	
					/// pgsql
					pqxx::Connection *pg;
	
					void cmd_dom_add();
					void cmd_dom_rm();
					void cmd_user_add();
					void cmd_user_pass();
					void cmd_user_rm();
					void cmd_user_auth();
					void cmd_user_ex();
					void cmd_dom_ip_add();
					void cmd_dom_ip_rm();
					void cmd_dom_ip_rm_all();
					void cmd_dom_ip_ls();
					void cmd_dom_ip_ls_dom();
					void cmd_udot_add() throw (out_of_range);
					void cmd_udot_ls();
					void cmd_udot_ls_type();
					void cmd_udot_rm();
					void cmd_udot_rm_type();
					void cmd_udot_get();
					void cmd_udot_rep();
					void cmd_udot_has();
					void cmd_udot_type_cnt();
					void cmd_qt_def_set();
					void cmd_qt_def_get();
					void cmd_qt_global_def_set();
					void cmd_qt_global_def_get();
					void cmd_qt_get();
					void cmd_qt_set();
	
					void qt_def_get(const std::string &);
					void qt_def_set(const std::string &);
	};

} // namespace POA_vq
