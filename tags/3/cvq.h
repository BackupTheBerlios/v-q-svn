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
		public:
				typedef uint32_t quota_value;
				typedef uint32_t size_type;

				/**
				 * @{
				 * \id except
				 * \title exceptions
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

				/// errors
				enum err {
						/// no error
						err_no = 0, 
						/// temporary error
						err_temp = 1,
						/// domain exists
						err_dom_ex,
						/// can't add to qmail/users/assign
						err_assign_add,
						/// can't add to qmail/control/rcpthosts
						err_rcpt_add,
						/// can't remove from qmail/control/locals
						err_locals_rm,
						/// can't add to qmail/control/virtauldomains
						err_virt_add,
						/// can't add domain to authorization module
						err_auth_dom_add,
						/// can't call qmail-newu
						err_assign_comp,
						/// user exists
						err_user_ex,
						/// can't add user to authorization module
						err_auth_user_add,
						/// can't remove user from authorization module
						err_auth_user_rm,
						/// invalid domain name
						err_dom_inv,
						/// invalid user name
						err_user_inv,
						/// domain was removed partially
						err_dom_rm_partial,
						/// user not found
						err_user_nf,
						/// no quota
						err_qt_no,
						/// can't set quota
						err_qt_sf,
						/// can't read quota
						err_qt_gf,
						/// can't change .qmail file for domain
						err_dom_dot,
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
						err_st,
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
						/// entry exists in dot file
						err_udot_ex
				};
				/**
				 * structure used in some routines. it includes basic
				 * user's information.
				 */
				typedef struct {
						std::string user,pass,dom;
						std::string dir;
						int flags;
				} auth_info;

				enum udot_type {
						redir = 'F', //!< redirection
						maildir = 'M', //!< maildir
						sms = 'S', //!< sms notification
						autoresp = 'R' //!< autorespond
				};
 
				/**
				 * user's dot file info
				 */
				typedef struct {
						std::string  id, //!< identificator
								val; //!< value
						udot_type type; //!< type of entry
				} udot_info;
				
				/**
				 * possible user account flags
				 */
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
 
				/*@{
				 * \id domains
				 * \title Playing with domains
				 */
			   
				/**
				 * Add domain
				 * \param domain name
				 */
				virtual uint8_t dom_add(const std::string &)=0;
				/**
				 * Add domain's alias address (for example ip address).
				 * Ip addresses should be in form [a.b.c.d], or a.b.c.d (
				 * it's better to add 2 forms).
				 * \param domain name
				 * \param alias
				 */
				virtual uint8_t dom_ip_add(const std::string &,const std::string &)=0;
				virtual uint8_t dom_ip_rm(const std::string &,const std::string &)=0;
				virtual uint8_t dom_ip_rm_all(const std::string &)=0;
				virtual uint8_t dom_ip_ls(const std::string &, std::vector<std::string> &)=0;
				virtual uint8_t dom_ip_ls_dom(std::vector<std::string> &)=0;
				virtual uint8_t dom_rm(const std::string &)=0;
				virtual uint8_t dom_alias_add(const std::string &, const std::string &) =0;
				virtual uint8_t dom_alias_rm(const std::string &) =0;
				virtual uint8_t dom_val(const std::string &)=0;
				/*}@*/

				virtual const char * err()=0;
				virtual const char * err_str( unsigned e ) {
						/// error descriptions
						static char * _err_str[] = {
						"No error",
						"Temporary error",
						"Domain exists",
						"Can't add to assign",
						"Can't add to rcpthosts",
						"Can't remove from locals",
						"Can't add to virtualdomains",
						"Can't add domain to authentication module",
						"Can't compile assign",
						"User exists",
						"Can't add user to authentication module",
						"Can't remove user from authentication module",
						"Invalid domain name",
						"Invalid user name",
						"Domain removed partially",
						"User not found",
						"No quota file or I can't access it",
						"Can't set quota",
						"Can't get quota",
						"Can't change .qmail file for domain",
						"Error in authorization module",
						"dot-file: Type not supported",
						"Can't open or create file",
						"Can't write",
						"Can't read",
						"Can't stat",
						"It's not a directory",
						"Chmod failed",
						"File is locked",
						"Can't rename",
						"Can't chown",
						"Dot file includes this entry"
						};
						if( e >= sizeof(_err_str) )
								return "Bad error number";
						return _err_str[e];
				}
				virtual uint8_t user_add(const std::string &, 
						const std::string &, 
						const std::string &, uint8_t = 0)=0;
				virtual uint8_t user_rm(const std::string &, const std::string &)=0;
				virtual uint8_t user_pass(const std::string &, const std::string &, 
						const std::string &)=0;
				virtual uint8_t user_val(const std::string &)=0;
				virtual uint8_t user_auth(auth_info &)=0;
				virtual uint8_t user_ex(const std::string &, const std::string &)=0;
				//virtual char user_flags(auth_info &)=0;
 
				/*@{
				 * \id qt
				 * \title quota support
				 */
				virtual std::string qtfile(const std::string &, const std::string &)=0;
				virtual std::string qtfile(const std::string &)=0;
				virtual std::string qtfile_def(const std::string &)=0;
				virtual std::string qtfile_def()=0;
				
				virtual uint8_t qt_cur_set(const std::string&, const std::string &, int32_t)=0;
				virtual uint8_t qt_set(const std::string &, const std::string &, quota_value )=0;
				virtual uint8_t qt_set(const std::string &, const std::string &, quota_value, quota_value )=0;
				virtual uint8_t qt_get(const std::string&, const std::string &, quota_value &, quota_value &)=0;
				virtual uint8_t qt_over(const std::string &, const std::string &)=0;
				virtual uint8_t qt_def_set(const std::string &, quota_value)=0;
				virtual uint8_t qt_def_get(const std::string &, quota_value &)=0;
				virtual uint8_t qt_def_cp(const std::string &, const std::string &)=0;

				virtual uint8_t qt_dom_cur_set(const std::string &, int32_t)=0;
				virtual uint8_t qt_dom_set(const std::string &, quota_value)=0;
				virtual uint8_t qt_dom_set(const std::string &, quota_value, quota_value)=0;
				virtual uint8_t qt_dom_over(const std::string &)=0;
				virtual uint8_t qt_dom_get(const std::string &, quota_value &, quota_value &)=0;
				virtual uint8_t qt_dom_def_set(quota_value)=0;
				virtual uint8_t qt_dom_def_get(quota_value &)=0;
				virtual uint8_t qt_dom_def_cp(const std::string &)=0;
				/*}@*/

				/*@{
				 * \id ud_info
				 * \title Playing with dot files
				 */
				/// add to dot file
				virtual uint8_t udot_add(const std::string &, const std::string &,
								const std::string &, udot_info & )=0;
				virtual uint8_t udot_add_md_def(const std::string &, const std::string &,
								const std::string &)=0;
				virtual uint8_t udot_rm(const std::string &, const std::string &,
								const std::string &, udot_type )=0;
				virtual uint8_t udot_rm(const std::string &, const std::string &,
								const std::string &, const std::string &)=0;
				virtual uint8_t udot_ls(const std::string &, const std::string &,
								const std::string &, std::vector<udot_info> & )=0;
				virtual uint8_t udot_ls(const std::string &, const std::string &,
								const std::string &, udot_type, 
								std::vector<udot_info> & )=0;
				virtual uint8_t udot_rep(const std::string &, const std::string &,
								const std::string &, const udot_info & )=0;
				virtual uint8_t udot_get(const std::string &, udot_info &)=0;
				virtual uint8_t udot_sup_is(udot_type)=0;
				virtual uint8_t udot_has(const std::string &, const std::string &,
								const std::string &, udot_type )=0;
				virtual uint8_t udot_type_cnt(const std::string &, const std::string &,
								const std::string &, udot_type, size_type & )=0;
				/*}@*/
};

#endif
