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
#include "../logger.hpp"
#include "pgsqlcommon.hpp"

#include <memory>
#include <algorithm>

namespace POA_vq {
	
	/**
	 *
	 */
	 class cpgsqllog : public ilogger {
			public:
					typedef ::vq::ilogger::service_type service_type;
					typedef ::vq::ilogger::result result;
					typedef ::vq::ilogger::error error;
					typedef ::vq::ilogger::size_type size_type;
					typedef ::vq::ilogger::log_entry_list_out log_entry_list_out;
					typedef ::vq::ilogger::log_entry_list log_entry_list;
					typedef ::vq::ilogger::err_code err_code;
					
					virtual void ip_set( const char* ip );
    				virtual void service_set( service_type ser );
    				virtual void domain_set( const char* dom );
    				virtual void login_set( const char* log );
    				virtual error* write( result r, const char* msg );
    				virtual error* read( size_type start, size_type cnt, 
							log_entry_list_out les );
					virtual error* count( size_type & cnt );
    				virtual error* read_by_dom( size_type start, size_type cnt, 
							log_entry_list_out les );
					virtual error* count_by_dom( size_type & cnt );
    				virtual error* read_by_user( size_type start, size_type cnt, 
							log_entry_list_out les );
					virtual error* count_by_user( size_type & cnt );
					virtual error* rm_all();
					virtual error* rm_by_dom();
					virtual error* rm_by_user();

					cpgsqllog( const char * );
					virtual ~cpgsqllog();

			protected:
					/// pgsql
					std::auto_ptr<pqxx::Connection> pg;

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

} // namespace POA_vq
