/*
Copyright (c) 2002,2003,2004 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
iMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
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
