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

#include <sys/types.h>

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
					typedef ::vq::ivq::err_code err_code;
					typedef ::vq::ivq::string_list_out string_list_out;
					typedef ::vq::ivq::email_banned_list_out email_banned_list_out;

					cqmailvq( const std::string &, unsigned, unsigned, 
						mode_t, mode_t, mode_t, 
						const std::string &, uid_t, gid_t );
					virtual ~cqmailvq() {}

					virtual error* dom_add( const char* dom );
				    virtual error* dom_rm( const char* dom );
				    virtual error* dom_val( const char* dom );
				    virtual error* dom_id( const char* dom, CORBA::String_out dom_id );
    				virtual error* dom_name( const char* dom_id, CORBA::String_out dom_name );
#if 0
				    virtual error* dra_add( const char* dom_id, const char* rea );
				    virtual error* dra_rm( const char* dom_id, const char* rea );
				    virtual error* dra_rm_by_dom( const char* dom_id );
				    virtual error* user_add( auth_info& ai, CORBA::Boolean is_banned );
				    virtual error* user_rm( const char* dom_id, const char* user_id );
				    virtual error* user_pass( const char* dom_id, const char* user_id, const char* pass );
				    virtual error* user_val( const char* user );
				    virtual error* user_auth( auth_info& ai );
				    virtual error* user_id( const char* dom_id, const char* login, CORBA::String_out uid );
    				virtual error* user_name( const char* dom_id, const char* uid, CORBA::String_out login );
				    virtual error* eb_add( const char* re_domain, const char* re_login );
				    virtual error* eb_rm( const char* re_domain, const char* re_login );
				    virtual error* eb_ls( email_banned_list_out ebs );
#endif // if 0
			protected:
					std::string home; //!< home directory
					unsigned dom_split;
					unsigned user_split;
					mode_t fmode; //!< permissions of created files
					mode_t mmode; //!< permissions of created directories in Maildir
					mode_t dmode; //!< permissions of directories created
					std::string user; //!< user name we are working as
					uid_t uid; //!< user's id.
					gid_t gid; //!< group's id.

					static const char tmp_end[]; //!< temporary file extension
					static const char ud_sup[]; //!< array of supported udot types
						
					std::auto_ptr< ::vq::iauth > auth; //!< authorization module
	
					error * assign_ex(const std::string &);
					error * assign_add(const std::string &);
					error * assign_rm(const std::string &);
					error * rcpt_add(const std::string &);
					error * rcpt_rm(const std::string &);
					error * morercpt_add(const std::string &);
					error * morercpt_rm(const std::string &);
					error * locals_rm(const std::string &);
					error * virt_add(const std::string &,const std::string &);
					error * virt_rm(const std::string &);
					int run( char * const [] );
				
					/// path to maildir
					std::string maildir_path(const std::string &, 
							const std::string &);
	
					/// Make maildir
					error * maildir_make(const std::string &);
			
				/**
				 * \defgroup file Files manipulation
				 */
				//@{
					/// Replace first line of file (or create new file)
					error * file_first_rep(const std::string &, const std::string &,
							std::string &);
					
					/// Adds line to a file
					error * file_add(const std::string &, const std::string &,
							std::string &, bool = false);
	
					/// Adds line to a file
					error * file_add(const std::string &fn, mode_t mode, 
							const std::string &eof_mark, const std::string &item_mark, 
							bool item_whole, const std::string &item_add, 
							std::string &fntmp);
	
					/// Removes line from a file
					error * file_rm(const std::string &, const std::string &,
							std::string &);
	
					/// Creates temporary file
					error * tmp_crt(const std::string &, posix::opfstream &, 
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
	
					error * dom_del_lock(const std::string &);
					error * dom_del_unlock(const std::string &); 
				//@}

				/**
				 * \defgroup err Errors handling
				 */
				/*@{*/
#define lr(ec, what) lr_wrap(ec, what, __FILE__, __LINE__) //!< return lastret
					inline error * lr_wrap(err_code, const std::string &, 
							const char *, CORBA::ULong );
					error * lr_wrap(err_code, const char *, 
							const char *, CORBA::ULong );
				/*@}*/	
	};

	/**
	 *
	 */
	inline cqmailvq::error * cqmailvq::lr_wrap(err_code ec, 
			const std::string & what, 
			const char *file, CORBA::ULong line) {
		return lr_wrap( ec, what.c_str(), file, line);
	}

} // namespace vq

#endif
