/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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

#ifndef __CVQ_H
#define __CVQ_H

#include <string>
#include <vector>
#include <exception>
#include <inttypes.h>

/**
 * Base class for all VQ operations. Authorization module was split to cauth 
 * class.
 */
class cvq {
		protected:
		/**
		 * \defgroup err Errors handling
		 */
		/*@{*/
				/// additional information (i.e. file name)
				std::string lr_info;
				int lr_errno; //!< errno

				uint8_t lastret; //!< last error code
				bool lastret_blkd; //!< when true lastret won't be changed
				uint8_t lr(uint8_t, const std::string &); //!< set lastret
		/*@}*/	

		public:
				typedef uint32_t quota_value; //!< Type for holding quota values
				typedef uint32_t size_type; //!< Type for holding sizes
				virtual ~cvq() {}
				cvq();

		/**
		 * \defgroup exceptions Exceptions used by this class
		 * \ingroup err
		 * @{
		 */
				/**
				 * runtime error
				 */
				class runtime_error : public std::exception {
						protected:
								std::string _what;
						public:
								runtime_error(const std::string &, 
									const std::string &, int ); 
								virtual const char *what() const throw();
								virtual ~runtime_error() throw() {}
				};
				
				/**
				 * you wrote some shit
				 */
				class logic_error : public std::exception {
						protected:
								std::string _what;
						public:
								logic_error(const std::string &,
									const std::string &, int);
								virtual const char *what() const throw();
								virtual ~logic_error() throw() {}
				};
		/*@}*/

		/**
		 * \ingroup err
		 */
		/*@{*/
				/// errors
				enum error {
						/// no error
						err_no = 0, 
						/// temporary error
						err_temp = 1,
						/// invalid domain name
						err_dom_inv,
						/// invalid user name
						err_user_inv,
						/// user not found
						err_user_nf,
						/// error in authorization module
						err_auth,
						/// dot file - type is not supported
						err_udot_tns,
						/// can't open sth
						err_open,
						/// can't write
						err_wr,
						/// can't read
						err_rd,
						/// can't stat
						err_stat,
						/// not a directory
						err_notdir,
						/// chmod failed
						err_chmod,
						/// file's locked
						err_lckd,
						/// can't rename
						err_ren,
						/// can't change owner
						err_chown,
						/// overflow
						err_over,
						/// child crashed or returned something unexpected
						err_exec,
						/// can't create directory
						err_mkdir,
						/// entry exists
						err_exists,
						/// no such element/entry
						err_noent
				};
		/*@}*/
				
				/// possible user account flags
				enum auth_info_flags {
						/// blocked pop3 access
						pop3_blk = 1,
						/// blocked smtp access
						smtp_blk = 2,
						/// admin (it was first added user)
						admin = 4,
						/// ftp access blocked
						ftp_blk = 8,
						/// ignore me (entry MUST be ignored)
						ignore = 16,
						/// super (server) admin
						supadm = 32
				};

				/**
				 * structure used in some routines. it includes basic
				 * user's information.
				 */
				typedef struct {
						std::string user; //!< user name
						std::string pass; //!< password
						std::string dom; //!< domain
						std::string dir; //!< mailbox directory
						auth_info_flags flags; //!< additional flags
				} auth_info;

				/// Mailbox options
				enum udot_type {
						redir = 'F', //!< redirection
						maildir = 'M', //!< maildir
						sms = 'S', //!< sms notification
						autoresp = 'R' //!< autorespond
				};
 
				/// user's dot file info
				typedef struct {
						std::string id; //!< identificator
						std::string val; //!< value
						udot_type type; //!< type of entry
				} udot_info;
				
		/**
		 * \defgroup dom Playing with domains
		 * @{
		 */
				/// Add domain
				virtual uint8_t dom_add(const std::string &)=0;

				/**
				 * Add domain's alias address (for example ip address).
				 * Ip addresses should be in form [a.b.c.d], or a.b.c.d (
				 * it's better to add 2 forms).
				 */
				virtual uint8_t dom_ip_add(const std::string &,const std::string &)=0;
				/// Removes IP address
				virtual uint8_t dom_ip_rm(const std::string &,const std::string &)=0;
				/// Removes all IP addresses
				virtual uint8_t dom_ip_rm_all(const std::string &)=0;
				/// Lists addresses asociated with domain
				virtual uint8_t dom_ip_ls(const std::string &, std::vector<std::string> &)=0;
				/// Lists domains which have IP addresses asociated
				virtual uint8_t dom_ip_ls_dom(std::vector<std::string> &)=0;
				/// Removes domain
				virtual uint8_t dom_rm(const std::string &)=0;
				/// Adds alias for domain
				virtual uint8_t dom_alias_add(const std::string &, const std::string &) =0;
				/// Removes alias
				virtual uint8_t dom_alias_rm(const std::string &) =0;
				/// Validates domain name
				virtual uint8_t dom_val(const std::string &);
		/*@}*/

		/**
		 * \ingroup err
		 */
		/*@{*/
				/// return additional information about error (i.e. file name)
				std::string err_info() const;
				int err_sys() const; //!< return stored errno

				/// return message associated with given error code
				static std::string err_str( error );
				/// return message associated with given error code
				static std::string err_str( uint8_t );

				/// return description of error from lastret
				std::string err_report() const;
		/*@}*/
				
		/**
		 * \defgroup user User handling
		 * @{
		 */
				/// Adds user
				virtual uint8_t user_add(const std::string &, 
						const std::string &, const std::string &, uint8_t = 0)=0;
				/// Removes user
				virtual uint8_t user_rm(const std::string &, const std::string &)=0;
				/// Changes password
				virtual uint8_t user_pass(const std::string &, const std::string &, 
						const std::string &)=0;
				/// Checks if user name is valid (not if such user exists)
				virtual uint8_t user_val(const std::string &);
				/// Gets authorization info. for user
				virtual uint8_t user_auth(auth_info &)=0;
				/// Checks if user exists
				virtual uint8_t user_ex(const std::string &, const std::string &)=0;
		//@}

		/**
		 * \defgroup qt Quota support
		 * @{
		 */
				//@{
				/** Returns path to quota file. */
				virtual std::string qtfile(const std::string &, const std::string &)=0;
				virtual std::string qtfile(const std::string &)=0;
				virtual std::string qtfile_def(const std::string &)=0;
				virtual std::string qtfile_def()=0;
				//@}
			
			/**
			 * \defgroup qt_user Quota support for users
			 * \ingroup qt
			 */
			//@{
				/// Adds value to current usage
				virtual uint8_t qt_cur_set(const std::string&, 
						const std::string &, int32_t)=0;
				/// Sets quota
				virtual uint8_t qt_set(const std::string &, 
						const std::string &, quota_value )=0;
				/// Sets max and current usage
				virtual uint8_t qt_set(const std::string &, 
						const std::string &, quota_value, quota_value )=0;
				/// Gets usage value
				virtual uint8_t qt_get(const std::string&, 
						const std::string &, quota_value &, quota_value &)=0;
				/// Checks if over quota
				virtual uint8_t qt_over(const std::string &, 
						const std::string &)=0;
				/// Sets default value
				virtual uint8_t qt_def_set(const std::string &, quota_value)=0;
				/// Gets default value
				virtual uint8_t qt_def_get(const std::string &, quota_value &)=0;
				/// Sets max. usage from default
				virtual uint8_t qt_def_cp(const std::string &, 
						const std::string &)=0;
			//@}
			
			/**
			 * \defgroup qt_dom Quota support for domains
			 */
			//@{
				/// Sets current usage for domain
				virtual uint8_t qt_dom_cur_set(const std::string &, int32_t)=0;
				/// Sets max usage
				virtual uint8_t qt_dom_set(const std::string &, quota_value)=0;
				/// Sets max and current usage
				virtual uint8_t qt_dom_set(const std::string &, quota_value, 
						quota_value)=0;
				/// Checks if over quota
				virtual uint8_t qt_dom_over(const std::string &)=0;
				/// Gets values of current and max
				virtual uint8_t qt_dom_get(const std::string &, quota_value &, 
						quota_value &)=0;
				/// Sets defaults for domains
				virtual uint8_t qt_dom_def_set(quota_value)=0;
				/// Gets defaults for domains
				virtual uint8_t qt_dom_def_get(quota_value &)=0;
				/// Copies from defaults
				virtual uint8_t qt_dom_def_cp(const std::string &)=0;
			//@}
		/*@}*/

		/**
		 * \defgroup udot Playing with user mailbox's options
		 * @{
		 */
				/// Adds to dot file
				virtual uint8_t udot_add(const std::string &, const std::string &,
						const std::string &, udot_info & )=0;
				/// Adds some default configuration to user's mailbox
				virtual uint8_t udot_add_md_def(const std::string &, const std::string &,
						const std::string &)=0;
				/// Removes all options of specified type
				virtual uint8_t udot_rm(const std::string &, const std::string &,
						const std::string &, udot_type )=0;
				/// Removes option with given id
				virtual uint8_t udot_rm(const std::string &, const std::string &,
						const std::string &, const std::string &)=0;
				/// Lists mailbox configuration
				virtual uint8_t udot_ls(const std::string &, 
						const std::string &, const std::string &, 
						std::vector<udot_info> & )=0;
				/// Lists mailbox options of specified type
				virtual uint8_t udot_ls(const std::string &, const std::string &,
						const std::string &, udot_type, 
						std::vector<udot_info> & )=0;
				/// Changes value of option with given id.
				virtual uint8_t udot_rep(const std::string &, const std::string &,
						const std::string &, const udot_info & )=0;
				/// Gets value of options with given id.
				virtual uint8_t udot_get(const std::string &, udot_info &)=0;
				/// Checks if given option is supported
				virtual uint8_t udot_sup_is(udot_type)=0;
				/// Checks if mailbox has configured option of given type
				virtual uint8_t udot_has(const std::string &, 
						const std::string &,
						const std::string &, udot_type )=0;
				/// Counts options with given type
				virtual uint8_t udot_type_cnt(const std::string &, 
						const std::string &,
						const std::string &, udot_type, size_type & )=0;
		/*@}*/
				static cvq * alloc(); //!< create implementation instance
};

#endif
