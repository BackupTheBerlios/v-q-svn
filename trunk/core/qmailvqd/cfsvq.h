/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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

#ifndef __CFSVQ_H
#define __CFSVQ_H

#include <stdexcept>
#include <string>
#include <memory>

#include "pfstream.h"
#include "cvq.h"
#include "cauth.h"

/**
 * Implementation of cvq. It stores everything on filesystem (in directory
 * conf_vq/domains/).
 */
class cfsvq : public cvq {
		public:
				cfsvq();
				virtual ~cfsvq(){}

			/**
			 * \ingroup dom
			 */
			//@{
				uint8_t dom_add(const std::string &);
				uint8_t dom_ip_add(const std::string &,const std::string &);
				uint8_t dom_ip_rm(const std::string &,const std::string &);
				uint8_t dom_ip_rm_all(const std::string &);
				uint8_t dom_ip_ls(const std::string &,std::vector<std::string> &);
				uint8_t dom_ip_ls_dom(std::vector<std::string> &);
				uint8_t dom_rm(const std::string &);
				uint8_t dom_alias_add(const std::string &,const std::string &);
				uint8_t dom_alias_rm(const std::string &);
			//@}

			/**
			 * \ingroup user
			 */
			//@{
				uint8_t user_add(const std::string &, const std::string &, 
								const std::string &, uint8_t = 0 );
				uint8_t user_pass(const std::string &,const std::string &,const std::string &);
				uint8_t user_rm(const std::string &,const std::string &);
				uint8_t user_auth(auth_info &);
				uint8_t user_ex(const std::string &, const std::string &);
			//@}

			/**
			 * \ingroup qt_user
			 */
			//@{
				uint8_t qt_cur_set(const std::string&, const std::string &, 
						int32_t);
				uint8_t qt_over(const std::string &, const std::string &);
				uint8_t qt_def_cp(const std::string &, const std::string &);

				uint8_t qt_set(const std::string &, const std::string &, 
						quota_value, quota_value );
				uint8_t qt_get(const std::string&, const std::string &, 
						quota_value &, quota_value &);

				uint8_t qt_def_get(const std::string &, quota_value &, 
						quota_value &);
				uint8_t qt_def_set(const std::string &, quota_value, 
						quota_value);

				uint8_t qt_global_def_set(quota_value, quota_value);
				uint8_t qt_global_def_get(quota_value &, quota_value &);
			//@}
				
			/**
			 * \ingroup udot
			 */
			//@{
				uint8_t udot_add(const std::string &, const std::string &,
								const std::string &, udot_info & );
				uint8_t udot_rm(const std::string &, const std::string &,
								const std::string &, const std::string &);
				uint8_t udot_ls(const std::string &, const std::string &,
								const std::string &, std::vector<udot_info> & );
				uint8_t udot_ls(const std::string &, const std::string &,
								const std::string &, udot_type,
								std::vector<udot_info> & );
				uint8_t udot_rep(const std::string &, const std::string &, 
								const std::string &, const udot_info & );
				uint8_t udot_get(const std::string &, udot_info &);
				uint8_t udot_sup_is(udot_type);
				uint8_t udot_has(const std::string &, const std::string &,
								const std::string &, udot_type );
				uint8_t udot_add_md_def(const std::string &, const std::string &,
								const std::string &);
				uint8_t udot_rm(const std::string &, const std::string &,
								const std::string &, udot_type );
				uint8_t udot_type_cnt(const std::string &, const std::string &,
								const std::string &, udot_type, size_type & );
			//@}

		protected:
				static const char tmp_end[]; //!< temporary file extension
				static const char ud_sup[]; //!< array of supported udot types
					
				std::auto_ptr<cauth> auth; //!< authorization module

				uint8_t assign_ex(const std::string &);
				uint8_t assign_add(const std::string &);
				uint8_t assign_rm(const std::string &);
				uint8_t rcpt_add(const std::string &);
				uint8_t rcpt_rm(const std::string &);
				uint8_t morercpt_add(const std::string &);
				uint8_t morercpt_rm(const std::string &);
				uint8_t locals_rm(const std::string &);
				uint8_t virt_add(const std::string &,const std::string &);
				uint8_t virt_rm(const std::string &);
				int run( char * const [] );
			
				/// path to maildir
				std::string maildir_path(const std::string &, 
						const std::string &);

				/// Make maildir
				uint8_t maildir_make(const std::string &);
		
			/**
			 * \defgroup file Files manipulation
			 */
			//@{
				/// Replace first line of file (or create new file)
				uint8_t file_first_rep(const std::string &, const std::string &,
						std::string &);
				
				/// Adds line to a file
				uint8_t file_add(const std::string &, const std::string &,
						std::string &, bool = false);

				/// Adds line to a file
				uint8_t file_add(const std::string &fn, mode_t mode, 
						const std::string &eof_mark, const std::string &item_mark, 
						bool item_whole, const std::string &item_add, uint8_t ex, 
						std::string &fntmp);

				/// Removes line from a file
				uint8_t file_rm(const std::string &, const std::string &,
						std::string &);

				/// Creates temporary file
				uint8_t tmp_crt(const std::string &, posix::opfstream &, 
						std::string & );
			//@}

				void assert_auth();
			
			/**
			 * \ingroup udot
			 */
			//@{
				/// create path to dot-qmail
				std::string dotfile(const std::string &, const std::string &, const std::string &);
				/// create line which will be added to dot-qmail
				std::string udot_ln(const udot_info &);
			//@}

			/**
			 * \ingroup dom
			 */
			//@{
				std::string dompath(const std::string &);

				uint8_t dom_del_lock(const std::string &);
				uint8_t dom_del_unlock(const std::string &); 
			//@}
};

#endif
