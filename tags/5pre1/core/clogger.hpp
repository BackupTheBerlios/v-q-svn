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

#ifndef __CLOG_H
#define __CLOG_H

#include <string>
#include <stdexcept>
#include <vector>
#include <inttypes.h>

/**
 * Base class for subsystem responsible of storing informations about
 * users' loggins.
 */
class clogger {
		public:
				/// Size
				typedef uint32_t size_type;

				/// what we're logging about
				enum service {
						ser_unknown=0, //!< unknown connection
						ser_smtp=1, //!< smpt connection
						ser_pop3=2, //!< pop3 connection
						ser_http=3, //!< http connection
						ser_https=4, //!< https connection
						ser_emtp=5 //!< my emtp protocol
				};

				/// status of authorization/login
				enum result {
						re_ok=0, //!< success
						re_read=1, //!< can't read data
						re_data=2, //!< invalid data (domain, username, password)
						re_pass=3, //!< invalid password, description includes given password
						re_int=4, //!< internal error
						re_blk=5 //!< user's blocked
				};

				/// information stored in log db
				typedef struct {
						std::string id; //!< entry's id
						std::string time; //!< time when entry was stored
						service ser; //!< service
						std::string msg; //!< error message?
						std::string login; //!< user's id
						std::string domain; //!< domain
						std::string ip; //!< client's ip
						result res; //!< login status, i.e.: ok, error, blocked
				} entry_type;
			
			/**
			 * \defgroup err Errors handling
			 */
			//@{
				/// Values returned by functions
				enum error {
						err_no = 0, //!< no errors 
						err_temp, //!< unknown error
						err_dom_log, //!< there's no log table for specified domain
						err_empty, //!< result empty
						err_rd, //!< read error
						err_wr //!< write error
				};
			//@}
				
				clogger();
				virtual ~clogger();

		/**
		 * \ingroup err
		 */
		/*@{*/
				/// return additional information about error (i.e. file name)
				std::string err_info() const;
				int err_sys() const;

				/// return message associated with given error code
				static std::string err_str( error );
				static std::string err_str( uint8_t );

				/// return description of error from lastret
				std::string err_report() const;
		/*@}*/

				/// sets client address
				void ip_set(const std::string &);

				/// set service type
				void service_set( service );

				/// set domain
				void domain_set( const std::string & );

				/// set user
				void login_set( const std::string & );
				
				/// Write information
				virtual uint8_t write( result r, const std::string & s ) = 0;

				/**
				 * Read all entries from global log (log table) where domain and
				 * login match.
				 */ 
				virtual uint8_t log_read_dom_log( std::vector<entry_type> &, 
						size_type = 0, size_type = 0) = 0;

				/**
				 * Count entries.
				 */
				virtual uint8_t log_cnt_dom_log( size_type & ) = 0;

				/**
				 * Read all entries from domain_log table where login matches.
				 */
				virtual uint8_t dom_log_read_log( std::vector<entry_type> &,
						size_type = 0, size_type = 0 ) = 0;

				/**
				 * Count entries.
				 */
				virtual uint8_t dom_log_cnt_log( size_type & ) = 0;

				/// use this to create instance
				static clogger * alloc();

		protected:
				service ser; //!< what we're logging about
				std::string dom; //!< log this info in table for given domain
				std::string ip; //!< client's ip
				std::string log; //!< User's login

		/**
		 * \ingroup err
		 */
		//@{
				/// additional information (i.e. file name)
				std::string lr_info;
				int lr_errno;

				uint8_t lastret;
				bool lastret_blkd; //!< when true lastret won't be changed
				uint8_t lr(uint8_t, const std::string & = "");
		//@}

};

#endif
