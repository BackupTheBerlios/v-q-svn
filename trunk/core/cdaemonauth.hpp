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

#ifndef __CDAEMONAUTH_H
#define __CDAEMONAUTH_H

#include <string>
#include <stdexcept>
#include <vector>
#include <sys/types.h>
#include <sys/un.h>

#include "cauth.h"

/**
 * Daemon implementation of cauth class. It connects to daemon via unix
 * socket, it sends all command to it. Daemon can store informations
 * in database or filesystem.
 */
class cdaemonauth : public cauth {
		public:
				
				/// Supported commands
				enum cmd {
						cmd_dom_add = 0,
						cmd_dom_rm,
						cmd_user_add,
						cmd_user_rm,
						cmd_user_pass,
						cmd_user_auth,
						cmd_dom_ip_add,
						cmd_dom_ip_rm,
						cmd_dom_ip_rm_all,
						cmd_dom_ip_ls,
						cmd_dom_ip_ls_dom,
						cmd_udot_add,
						cmd_udot_rm,
						cmd_udot_ls,
						cmd_udot_ls_type,
						cmd_udot_rep,
						cmd_udot_get,
						cmd_udot_has,
						cmd_udot_rm_type,
						cmd_udot_type_cnt,
						cmd_user_ex,
						cmd_qt_def_set,
						cmd_qt_def_get,
						cmd_qt_global_def_set,
						cmd_qt_global_def_get,
						cmd_qt_get,
						cmd_qt_set
				};

				cdaemonauth();
				virtual ~cdaemonauth();

			/**
			 * \ingroup dom
			 */
			//@{
				uint8_t dom_add(const std::string &);
				uint8_t dom_ip_add(const std::string&,const std::string &);
				uint8_t dom_ip_rm(const std::string&,const std::string &);
				uint8_t dom_ip_ls(const std::string&,std::vector<std::string> &);
				uint8_t dom_ip_ls_dom(std::vector<std::string> &);
				uint8_t dom_ip_rm_all(const std::string&);
				uint8_t dom_rm(const std::string &);
			//}@
			
			/**
			 * \ingroup user
			 */
			//@{
				uint8_t user_add(const std::string &, const std::string &, 
								const std::string &, uint8_t = 0 );
				uint8_t user_rm(const std::string &,const std::string &);
				uint8_t user_pass(const std::string&,const std::string&, const std::string&);
				uint8_t user_auth(cvq::auth_info &);
				uint8_t user_ex(const std::string &, const std::string &);
			//}@
			
			/**
			 * \ingroup qt_user
			 */
			//@{
				/// Sets default value for a domain
				virtual uint8_t qt_def_set(const std::string &, 
						quota_value, quota_value);
				/// Gets default value
				virtual uint8_t qt_def_get(const std::string &, 
						quota_value &, quota_value &);

				/// Sets global defaults for new domains
				virtual uint8_t qt_global_def_set(quota_value, 
						quota_value);
				/// Gets global defaults for new domains
				virtual uint8_t qt_global_def_get(quota_value &, 
						quota_value &);

				/// Gets limits for user
				virtual uint8_t qt_get( const std::string &, 
						const std::string &, 
						quota_value &, quota_value & );

				/// Sets limits for user
				virtual uint8_t qt_set( const std::string &, 
						const std::string &, 
						quota_value, quota_value );
			//@}

			/**
			 * \ingroup udot
			 */
			//@{
				uint8_t udot_add(const std::string &, const std::string &,
						const std::string &, cvq::udot_info & );
				uint8_t udot_rm(const std::string &, const std::string &);
				uint8_t udot_rm(const std::string &, const std::string &,
						const std::string &, cvq::udot_type );
				uint8_t udot_ls(const std::string &, const std::string &,
						const std::string &, std::vector<cvq::udot_info> & );
				uint8_t udot_ls(const std::string &, const std::string &,
						const std::string &, cvq::udot_type,
						std::vector<cvq::udot_info> & );
				uint8_t udot_rep(const std::string &, const cvq::udot_info & );
				uint8_t udot_get(const std::string &,
						cvq::udot_info & );
				uint8_t udot_has(const std::string &, const std::string &, const std::string &,
						cvq::udot_type );
				uint8_t udot_type_cnt(const std::string &, const std::string &, 
						const std::string &, cvq::udot_type, cvq::size_type & );
			//}@

		protected:
				char cmd; //!< Common variable
				char resp; //!< Common variable
				int cso; //!< client's socket
				struct sockaddr_un cun; //!< client's address
};

#endif
