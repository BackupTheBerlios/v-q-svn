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

#ifndef __CQMAILVQ_HPP
#define __CQMAILVQ_HPP

#include "../vq.hpp"
#include "../auth.hpp"

#include <pfstream.hpp>

#include <stdexcept>
#include <string>
#include <memory>

namespace POA_vq {

	/**
	 * Implementation of ivq. It stores everything on filesystem (in directory
	 * conf_home/domains/).
	 */
	class cqmailvq : public ivq {
			public:
					typedef ::vq::ivq::auth_info auth_info;
					typedef ::vq::ivq::quota_type quota_type;
					typedef ::vq::ivq::quota_type_out quota_type_out;
					typedef ::vq::ivq::size_type size_type;
					typedef ::vq::ivq::size_type_out size_type_out;
					typedef ::vq::ivq::udot_type udot_type;
					typedef ::vq::ivq::udot_type_out udot_type_out;
					typedef ::vq::ivq::udot_info udot_info;
					typedef ::vq::ivq::udot_info_out udot_info_out;
					typedef ::vq::ivq::udot_info_list_out udot_info_list_out;
					typedef ::vq::ivq::error error;
					typedef ::vq::ivq::string_list_out string_list_out;

			
					cqmailvq( const std::string &, unsigned );
					virtual ~cqmailvq() {}

					virtual error dom_add( const char* dom );
					virtual error dom_ip_add( const char* dom, const char* ip );
					virtual error dom_ip_rm( const char* dom, const char* ip );
					virtual error dom_ip_rm_all( const char* dom );
					virtual error dom_ip_ls( const char* dom, string_list_out ips );
					virtual error dom_ip_ls_dom( string_list_out doms );
					virtual error dom_rm( const char* dom );
					virtual error dom_alias_add( const char* dom, const char* ali );
					virtual error dom_alias_rm( const char* ali );
					virtual error dom_val( const char* dom );
					virtual char* err_info();
					virtual CORBA::Long err_sys();
					virtual char* err_str( error err );
					virtual char* err_report();
    				virtual error user_add( const auth_info& ai, error eex );
					virtual error user_rm( const char* dom, const char* user );
					virtual error user_pass( const char* dom, const char* user, const char* pass );
					virtual error user_val( const char* user );
					virtual error user_auth( auth_info& ai );
					virtual error user_ex( const char* dom, const char* user );
					virtual error qt_parse( const char* qts, quota_type_out bytes_max, quota_type_out files_max );
					virtual error qt_set( const char* dom, const char* user, quota_type_out bytes_max, quota_type_out files_max );
					virtual error qt_get( const char* dom, const char* user, quota_type_out bytes_max, quota_type_out files_max );
					virtual error qt_def_set( const char* dom, quota_type bytes_max, quota_type files_max );
					virtual error qt_def_get( const char* dom, quota_type_out bytes_max, quota_type_out files_max );
					virtual error qt_global_def_set( quota_type bytes_max, quota_type files_max );
					virtual error qt_global_def_get( quota_type_out bytes_max, quota_type_out files_max );
					virtual error udot_add( const char* dom, const char* user, const char* pfix, udot_info_out ui );
					virtual error udot_add_md_def( const char* dom, const char* user, const char* pfix );
    				virtual error udot_rm_by_type( const char* dom, const char* user, udot_type ut );
    				virtual error udot_rm_by_id( const char* dom, const char* user, const char* id );
					virtual error udot_ls( const char* dom, const char* user, const char* pfix, udot_info_list_out uis );
					virtual error udot_ls_by_type( const char* dom, const char* user, const char* pfix, udot_type ut, udot_info_list_out uis );
    				virtual error udot_rep( const char* dom, const char* user, const udot_info& ui );
    				virtual error udot_get( const char* dom, const char* user, udot_info& ui );
					virtual error udot_sup_is( udot_type ut );
					virtual error udot_has( const char* dom, const char* user, const char* pfix, udot_type ut );
					virtual error udot_type_cnt( const char* dom, const char* user, const char* pfix, udot_type ut, size_type_out cnt );

			protected:
					std::string home; //!< home directory
					unsigned dom_split;


					static const char tmp_end[]; //!< temporary file extension
					static const char ud_sup[]; //!< array of supported udot types
						
					std::auto_ptr< ::vq::iauth > auth; //!< authorization module
	
					error assign_ex(const std::string &);
					error assign_add(const std::string &);
					error assign_rm(const std::string &);
					error rcpt_add(const std::string &);
					error rcpt_rm(const std::string &);
					error morercpt_add(const std::string &);
					error morercpt_rm(const std::string &);
					error locals_rm(const std::string &);
					error virt_add(const std::string &,const std::string &);
					error virt_rm(const std::string &);
					int run( char * const [] );
				
					/// path to maildir
					std::string maildir_path(const std::string &, 
							const std::string &);
	
					/// Make maildir
					error maildir_make(const std::string &);
			
				/**
				 * \defgroup file Files manipulation
				 */
				//@{
					/// Replace first line of file (or create new file)
					error file_first_rep(const std::string &, const std::string &,
							std::string &);
					
					/// Adds line to a file
					error file_add(const std::string &, const std::string &,
							std::string &, bool = false);
	
					/// Adds line to a file
					error file_add(const std::string &fn, mode_t mode, 
							const std::string &eof_mark, const std::string &item_mark, 
							bool item_whole, const std::string &item_add, error ex, 
							std::string &fntmp);
	
					/// Removes line from a file
					error file_rm(const std::string &, const std::string &,
							std::string &);
	
					/// Creates temporary file
					error tmp_crt(const std::string &, posix::opfstream &, 
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
	
					error dom_del_lock(const std::string &);
					error dom_del_unlock(const std::string &); 
				//@}

				/**
				 * \defgroup err Errors handling
				 */
				/*@{*/
					/// additional information (i.e. file name)
					std::string lr_info;
					int lr_errno; //!< errno
	
					error lastret; //!< last error code
					bool lastret_blkd; //!< when true lastret won't be changed
					error lr(error, const std::string &); //!< set lastret
				/*@}*/	
	};

} // namespace vq

#endif