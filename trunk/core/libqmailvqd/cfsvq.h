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
