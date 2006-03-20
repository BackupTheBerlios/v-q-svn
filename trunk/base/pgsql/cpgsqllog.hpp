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
#ifndef __CPGSQLLOG_HPP
#define __CPGSQLLOG_HPP

#include "../logger.hpp"
#include "pgsqlcommon.hpp"
#include "cpgsqlpool.hpp"

#include <boost/shared_ptr.hpp>

#include <memory>
#include <algorithm>

namespace POA_vq {
	
	/**
	 *
	 */
	 class cpgsqllog : public ilogger,
		public PortableServer::RefCountServantBase {
			public:
					typedef ::vq::ilogger::service_type service_type;
					typedef ::vq::ilogger::result_type result_type;
					typedef ::vq::ilogger::error error;
					typedef ::vq::ilogger::size_type size_type;
					typedef ::vq::ilogger::log_entry_list_out log_entry_list_out;
					typedef ::vq::ilogger::log_entry_list log_entry_list;
					typedef ::vq::ilogger::err_code err_code;
					typedef ::vq::ilogger::string_list2_out string_list2_out;
					typedef ::vq::ilogger::string_list2 string_list2;
					typedef ::vq::ilogger::string_list string_list;
					typedef ::vq::ilogger::string_list_out string_list_out;
					typedef ::vq::ilogger::service_type_list_out service_type_list_out;
					typedef ::vq::ilogger::service_type_list service_type_list;
					typedef ::vq::ilogger::result_type_list_out result_type_list_out;
					typedef ::vq::ilogger::result_type_list result_type_list;

					struct service_conf {
						std::string pg_info;
						size_type pgs_pool;

						bool read_by_sql;
						bool rm_by_sql;

						service_conf( const std::string & pg_info,
							size_type pgs_pool, bool read_by_sql, bool rm_by_sql )
							: pg_info(pg_info), pgs_pool(pgs_pool),
							read_by_sql(read_by_sql), rm_by_sql(rm_by_sql) {
						}
					};
				
					virtual void clear();
					virtual void ip_set( const char* ip );
    				virtual void service_set( service_type ser );
    				virtual void domain_set( const char* dom );
    				virtual void login_set( const char* log );
    				virtual error* write( result_type r, const char* msg );
    				virtual error* read( size_type start, size_type cnt, 
							log_entry_list_out les );
					virtual error* count( size_type & cnt );
    				virtual error* read_by_dom( size_type start, size_type cnt, 
							log_entry_list_out les );
					virtual error* count_by_dom( size_type & cnt );
    				virtual error* read_by_user( size_type start, size_type cnt, 
							log_entry_list_out les );
					virtual error* count_by_user( size_type & cnt );
					virtual error* read_by_sql(const char* sql, size_type start, 
							size_type cnt, string_list2_out les);
					virtual error* rm_all();
					virtual error* rm_by_dom();
					virtual error* rm_by_user();
					virtual error* rm_by_sql(const char* where);

					virtual error* dom_ls(string_list_out doms);
					virtual error* user_ls_by_dom(string_list_out user);
					virtual error* service_ls(service_type_list_out stl);
					virtual error* result_ls(result_type_list_out rtl);
					virtual error* ip_ls(string_list_out ips);

					cpgsqllog( const service_conf & );
					virtual ~cpgsqllog();

			protected:
					/// pgsql
					cpgsqlpool pool;

					service_conf conf;

					std::string ip;
					std::string dom;
					std::string log;
					std::string ser;

					typedef int rbf_ignore_type;
					const static rbf_ignore_type rbf_ignore_domain = 1;
					const static rbf_ignore_type rbf_ignore_login = 1<<1;

					typedef std::pair<rbf_ignore_type, std::string> 
							rbf_func_desc_type;
					
					virtual error* read_by_func( 
							const rbf_func_desc_type &,
							size_type start, size_type cnt, 
							log_entry_list_out les );

					virtual error* count_by_func( const std::string &,
							size_type & cnt );

					virtual error * rm_by_func( const std::string & );

					error * string_ls_by_sql( string_list_out reas, 
						const std::string & field, const std::string & view,
						const std::string & where = "" );

					template< typename TLO, typename TL, typename I >
					error * field_ls_by_sql( TLO reas, 
						const std::string & field, const std::string & view,
						const std::string & where = "" );

				/**
				 * \defgroup err Errors handling
				 */
				/*@{*/
#define lr(ec, what) lr_wrap(ec, what, __FILE__, __LINE__) //!< return lastret
					inline error * lr_wrap(err_code ec, const std::string &w, 
							const char *f, CORBA::ULong l) {
						return lr_wrap(ec, w.c_str(), f, l);
					}
					error * lr_wrap(err_code, const char *, 
							const char *, CORBA::ULong );
				/*@}*/	
	};

} // namespace POA_vq

#endif // ifndef __CPGSQLLOG_HPP
