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

#ifndef __CDAEMONVQ_H
#define __CDAEMONVQ_H

#include "cvq.h"

#include <inttypes.h>

#include <string>
#include <vector>
#include <stdexcept>

/**
 * All operations are done by daemon. This class handles connections.
 */
class cdaemonvq : public cvq {
		public:
				/// Supported commands
				enum cmd {
						cmd_dom_add = 0,
						cmd_dom_ip_add,
						cmd_dom_ip_rm,
						cmd_dom_ip_rm_all,
						cmd_dom_ip_ls,
						cmd_dom_ip_ls_dom,
						cmd_dom_rm,
						cmd_dom_alias_add,
						cmd_dom_alias_rm,
						cmd_dom_val,
						cmd_user_add,
						cmd_user_rm,
						cmd_user_pass,
						cmd_user_val,
						cmd_user_auth,
						cmd_user_ex,
						cmd_qt_cur_set,
						cmd_qt_max_set,
						cmd_qt_set,
						cmd_qt_get,
						cmd_qt_over,
						cmd_qt_def_set,
						cmd_qt_def_get,
						cmd_qt_def_cp,
						cmd_qt_dom_cur_set,
						cmd_qt_dom_max_set,
						cmd_qt_dom_set,
						cmd_qt_dom_over,
						cmd_qt_dom_get,
						cmd_qt_dom_def_set,
						cmd_qt_dom_def_get,
						cmd_qt_dom_def_cp,
						cmd_udot_add,
						cmd_udot_add_md_def,
						cmd_udot_rm,
						cmd_udot_rm_id,
						cmd_udot_ls,
						cmd_udot_ls_type,
						cmd_udot_rep,
						cmd_udot_get,
						cmd_udot_sup_is,
						cmd_udot_has,
						cmd_udot_type_cnt
				};

		/**
		 * \defgroup err Errors handling
		 */
		//@{
				class open_error;
				class chdir_error;
				class sysfun_error;
		//@}
				
				virtual ~cdaemonvq();
				cdaemonvq();

		/**
		 * \ingroup dom
		 * @{
		 */
				/// Add domain
				virtual uint8_t dom_add(const std::string &);

				/**
				 * Add domain's alias address (for example ip address).
				 * Ip addresses should be in form [a.b.c.d], or a.b.c.d (
				 * it's better to add 2 forms).
				 */
				virtual uint8_t dom_ip_add(const std::string &,
						const std::string &);
				/// Removes IP address
				virtual uint8_t dom_ip_rm(const std::string &,
						const std::string &);
				/// Removes all IP addresses
				virtual uint8_t dom_ip_rm_all(const std::string &);
				/// Lists addresses asociated with domain
				virtual uint8_t dom_ip_ls(const std::string &, 
						std::vector<std::string> &);
				/// Lists domains which have IP addresses asociated
				virtual uint8_t dom_ip_ls_dom(std::vector<std::string> &);
				/// Removes domain
				virtual uint8_t dom_rm(const std::string &);
				/// Adds alias for domain
				virtual uint8_t dom_alias_add(const std::string &, 
						const std::string &);
				/// Removes alias
				virtual uint8_t dom_alias_rm(const std::string &);
				/// Validates domain name
				virtual uint8_t dom_val(const std::string &);
		/*@}*/

		/**
		 * \ingroup user
		 * @{
		 */
				/// Adds user
				virtual uint8_t user_add(const std::string &, 
						const std::string &, const std::string &, uint8_t = 0);
				/// Removes user
				virtual uint8_t user_rm(const std::string &, 
						const std::string &);
				/// Changes password
				virtual uint8_t user_pass(const std::string &, 
						const std::string &, const std::string &);
				/// Checks if user name is valid (not if such user exists)
				virtual uint8_t user_val(const std::string &);
				/// Gets authorization info. for user
				virtual uint8_t user_auth(auth_info &);
				/// Checks if user exists
				virtual uint8_t user_ex(const std::string &, 
						const std::string &);
		//@}

		/**
		 * \ingroup qt
		 * @{
		 */
			/**
			 * \ingroup qt_user
			 * \ingroup qt
			 */
			//@{
				/// Adds value to current usage
				virtual uint8_t qt_cur_set(const std::string&, 
						const std::string &, int32_t);
				/// Sets quota
				virtual uint8_t qt_set(const std::string &, 
						const std::string &, quota_value );
				/// Sets max and current usage
				virtual uint8_t qt_set(const std::string &, 
						const std::string &, quota_value, quota_value );
				/// Gets usage value
				virtual uint8_t qt_get(const std::string&, 
						const std::string &, quota_value &, quota_value &);
				/// Checks if over quota
				virtual uint8_t qt_over(const std::string &, 
						const std::string &);
				/// Sets default value
				virtual uint8_t qt_def_set(const std::string &, quota_value);
				/// Gets default value
				virtual uint8_t qt_def_get(const std::string &, quota_value &);
				/// Sets max. usage from default
				virtual uint8_t qt_def_cp(const std::string &, 
						const std::string &);
			//@}
			
			/**
			 * \ingroup qt_dom
			 */
			//@{
				/// Sets current usage for domain
				virtual uint8_t qt_dom_cur_set(const std::string &, int32_t);
				/// Sets max usage
				virtual uint8_t qt_dom_set(const std::string &, quota_value);
				/// Sets max and current usage
				virtual uint8_t qt_dom_set(const std::string &, quota_value, 
						quota_value);
				/// Checks if over quota
				virtual uint8_t qt_dom_over(const std::string &);
				/// Gets values of current and max
				virtual uint8_t qt_dom_get(const std::string &, quota_value &, 
						quota_value &);
				/// Sets defaults for domains
				virtual uint8_t qt_dom_def_set(quota_value);
				/// Gets defaults for domains
				virtual uint8_t qt_dom_def_get(quota_value &);
				/// Copies from defaults
				virtual uint8_t qt_dom_def_cp(const std::string &);
			//@}
		/*@}*/

		/**
		 * \ingroup udot
		 * @{
		 */
				/// Adds to dot file
				virtual uint8_t udot_add(const std::string &, const std::string &,
						const std::string &, udot_info & );
				/// Adds some default configuration to user's mailbox
				virtual uint8_t udot_add_md_def(const std::string &, 
						const std::string &, const std::string &);
				/// Removes all options of specified type
				virtual uint8_t udot_rm(const std::string &, 
						const std::string &, const std::string &, udot_type );
				/// Removes option with given id
				virtual uint8_t udot_rm(const std::string &, const std::string &,
						const std::string &, const std::string &);
				/// Lists mailbox configuration
				virtual uint8_t udot_ls(const std::string &, 
						const std::string &, const std::string &, 
						std::vector<udot_info> & );
				/// Lists mailbox options of specified type
				virtual uint8_t udot_ls(const std::string &, const std::string &,
						const std::string &, udot_type, 
						std::vector<udot_info> & );
				/// Changes value of option with given id.
				virtual uint8_t udot_rep(const std::string &, const std::string &,
						const std::string &, const udot_info & );
				/// Gets value of options with given id.
				virtual uint8_t udot_get(const std::string &, udot_info &);
				/// Checks if given option is supported
				virtual uint8_t udot_sup_is(udot_type);
				/// Checks if mailbox has configured option of given type
				virtual uint8_t udot_has(const std::string &, 
						const std::string &,
						const std::string &, udot_type );
				/// Counts options with given type
				virtual uint8_t udot_type_cnt(const std::string &, 
						const std::string &,
						const std::string &, udot_type, size_type & );
		/*@}*/
				
		protected:
				char cmd; //!< Command send to server
				char resp; //!< Response code received from server
				int sock; //!< Socket to server
				struct sockaddr_un sock_un;
				
};

/**
 *
 */
class cdaemonvq::open_error : public std::exception {
		public:
				open_error(const char *);
				const char * what() const throw() {
					try { return msg.c_str(); } catch(...) {return "fuck!";}
				}
				int err_sys() const { return err_sys; }
				const std::string & fn_get() const { return fn; }

		protected:
				std::string msg;
				std::string fn;
				int err_sys;
};

/**
 *
 */
class cdaemonvq::chdir_error::chdir_error : public open_error {
		public:
				chdir_error(const char *dir)
						: open_error(dir) {}
};

/**
 * System function returned error, errno saved
 */
class cdaemonvq::sysfun_error::sysfun_error : public open_error {
		public:
				sysfun_error(const char *fun)
						: open_error(fun) {}
};

#endif // ifndef __CDAEMONVQ_H
