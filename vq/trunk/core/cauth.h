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
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#ifndef __CAUTH_H
#define __CAUTH_H

#include <string>
#include <vector>

#include "cvq.h"

/**
 * Base class representing authorization module.
 */
class cauth {
		public:
				/// Type used for size specifications, the same as in cvq
				typedef cvq::size_type size_type;
				typedef cvq::quota_value quota_value;

				cauth();
				virtual ~cauth();
			
			/**
			 * \defgroup dom Domains manipulation
			 */
			//@{
				/// Adds domain to database
				virtual uint8_t dom_add(const std::string &)=0;
				/// Adds domain's IP address
				virtual uint8_t dom_ip_add(const std::string &,
						const std::string &)=0;
				/// Remove IP for domain
				virtual uint8_t dom_ip_rm(const std::string &,
						const std::string &)=0;
				/// Removes all IP addresses asociated with domain
				virtual uint8_t dom_ip_rm_all(const std::string &)=0;
				/// Lists all IP addresses asociated with domain
				virtual uint8_t dom_ip_ls(const std::string &,
						std::vector<std::string> &)=0;
				/// Lists domains having IP addresses
				virtual uint8_t dom_ip_ls_dom(std::vector<std::string> &)=0;
				/// Removes domain.
				virtual uint8_t dom_rm(const std::string &)=0;
			//@}
			
			/**
			 * \defgroup user Users manipulation
			 */
			//@{
				/// Adds user
				virtual uint8_t user_add(const std::string &, 
						const std::string &, const std::string &, uint8_t = 0 )=0;
				/// Removes user
				virtual uint8_t user_rm(const std::string &,const std::string &)=0;
				/// Changes password for user
				virtual uint8_t user_pass(const std::string&,const std::string&,
						const std::string&)=0;
				/// Gets authorization information for user
				virtual uint8_t user_auth(cvq::auth_info &)=0;
				/// Checks whether user exists
				virtual uint8_t user_ex(const std::string &, 
						const std::string &)=0;
			//@}
			
			/**
			 * \ingroup qt_user
			 */
			//@{
				/// Gets limits for user
				virtual uint8_t qt_get(const std::string &,
						const std::string &, 
						quota_value &, quota_value &) = 0;
				/// Sets limits for user
				virtual uint8_t qt_set(const std::string &,
						const std::string &, 
						quota_value, quota_value) = 0;

				/// Sets default value for a domain
				virtual uint8_t qt_def_set(const std::string &, 
						cvq::quota_value, cvq::quota_value)=0;
				/// Gets default value
				virtual uint8_t qt_def_get(const std::string &, 
						cvq::quota_value &, cvq::quota_value &)=0;
				
				/// Sets global defaults for new domains
				virtual uint8_t qt_global_def_set(cvq::quota_value, 
						cvq::quota_value)=0;
				/// Gets global defaults for new domains
				virtual uint8_t qt_global_def_get(cvq::quota_value &, 
						cvq::quota_value &)=0;
			//@}

			/**
			 * \defgroup udot Mailboxes configuration
			 */
			//@{
				/// Adds mailbox configuration
				virtual uint8_t udot_add(const std::string &, const std::string &,
						const std::string &, cvq::udot_info & )=0;
				/// Removes all mailbox configuration entries
				virtual uint8_t udot_rm(const std::string &, 
						const std::string &)=0;
				/// Removes mailbox configuration entry
				virtual uint8_t udot_rm(const std::string &, const std::string &,
						const std::string &, cvq::udot_type )=0;
				/// Lists mailbox configuration
				virtual uint8_t udot_ls(const std::string &, 
						const std::string &,
						const std::string &, std::vector<cvq::udot_info> & )=0;
				/// Lists mailbox configuation of specified type
				virtual uint8_t udot_ls(const std::string &, const std::string &,
						const std::string &, cvq::udot_type ,
						std::vector<cvq::udot_info> & )=0;
				/// Changes existing entry
				virtual uint8_t udot_rep(const std::string &, 
						const cvq::udot_info & )=0;
				/// Gets existing entry
				virtual uint8_t udot_get(const std::string &,
						cvq::udot_info & )=0;
				/// Checks whether mailbox has entry of specified type
				virtual uint8_t udot_has(const std::string &, const std::string &,
						const std::string &, cvq::udot_type )=0;
				/// Counts entries of specified type
				virtual uint8_t udot_type_cnt(const std::string &, const std::string &,
						const std::string &, cvq::udot_type, cvq::size_type & )=0;
			//@}
			
				// function used to create objects
				static cauth * alloc();

			/**
			 * \ingroup err
			 */
			/*@{*/
				/// return additional information about error (i.e. file name)
				std::string err_info() const;
				int err_sys() const; //!< Returns stored errno

				/// return description of error from lastret
				std::string err_report() const;
			/*@}*/
/*****************************************/
		protected:
			/**
			 * \defgroup err Errors handling
			 */
			//@{
				int lr_errno; //!< errno at time of error
				std::string lr_info; //!< additional information
				
				uint8_t lastret; //!< last error code
				bool lastret_blkd; //!< when true lastret won't be changed
				uint8_t lr(uint8_t, const std::string & = ""); //!< set lastret
			//@}
};

#endif