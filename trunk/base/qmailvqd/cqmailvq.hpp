/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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

#include "qmail_files.hpp"
#include "cpaths.hpp"
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
	class cqmailvq : public ivq,
		public PortableServer::RefCountServantBase {
			public:
					typedef ::vq::ivq::user_info user_info;
					typedef ::vq::ivq::user_info_list user_info_list;
					typedef ::vq::ivq::user_info_list_out user_info_list_out;
					typedef ::vq::ivq::quota_type quota_type;
					typedef ::vq::ivq::quota_type_out quota_type_out;
					typedef ::vq::ivq::size_type size_type;
					typedef ::vq::ivq::size_type_out size_type_out;
					typedef ::vq::ivq::user_conf_type user_conf_type;
					typedef ::vq::ivq::user_conf_type_out user_conf_type_out;
					typedef ::vq::ivq::user_conf_info user_conf_info;
					typedef ::vq::ivq::user_conf_info_out user_conf_info_out;
					typedef ::vq::ivq::user_conf_info_list_out user_conf_info_list_out;
					typedef ::vq::ivq::user_conf_info_list_var user_conf_info_list_var;
					typedef ::vq::ivq::error error;
					typedef ::vq::ivq::err_code err_code;
					typedef ::vq::ivq::string_list_out string_list_out;
					typedef ::vq::ivq::string_list_var string_list_var;
					typedef ::vq::ivq::email_banned_list_out email_banned_list_out;
					typedef ::vq::ivq::domain_info_list_out domain_info_list_out;
					typedef ::vq::ivq::id_type id_type;

					/**
					 *
					 */
					struct service_conf {
						typedef mode_t mode_type;
						typedef uid_t uid_type;
						typedef gid_t gid_type;
						typedef vq::cpaths::size_type size_type;
						
						std::string home; //!< home directory
						std::string domains; //!< maildirs are placed here
						std::string deleted; //!< removed maildirs are placed here
						size_type backup_deleted; //!< backup deleted maildirs? yes if > 0
						mode_t fmode; //!< permissions of created files
						mode_t mmode; //!< permissions of created directories in Maildir
						mode_t dmode; //!< permissions of directories created
						std::string user; //!< user name we are working as
						uid_t uid; //!< user's id.
						gid_t gid; //!< group's id.
						vq::cpaths paths; //!< path generator (some common code)

						std::string iauth_import; //!< import instructions for importing iauth
				
						service_conf( const std::string &h, const std::string &d, 
							const std::string &del, size_type bdel,
							size_type s_dom, size_type s_user, 
							const std::string &md,
							mode_t fm, mode_t mm, mode_t dm,
							const std::string & user, uid_t uid, gid_t gid,
							const std::string & ia_imp ) 
							: home(h), domains(d), deleted(del), backup_deleted(bdel),
							fmode(fm), mmode(mm), dmode(dm), user(user), uid(uid), 
							gid(gid), paths(d, s_dom, s_user, md),
							iauth_import(ia_imp) {
						}
					};

					cqmailvq( const service_conf & );
					virtual ~cqmailvq() {}

					virtual error* dom_add( const char* dom, id_type & dom_id );
				    virtual error* dom_rm( id_type dom_id );
				    virtual error* dom_val( const char* dom );
				    virtual error* dom_id( const char* dom, id_type & dom_id );
    				virtual error* dom_name( id_type dom_id, CORBA::String_out dom_name );
					virtual error* dom_ls( domain_info_list_out dis );

				    virtual error* user_add( const user_info& ai, 
							CORBA::Boolean is_banned );
				    virtual error* user_rep( const user_info& ai, 
							CORBA::Boolean password, CORBA::Boolean home );
				    virtual error* user_rm( id_type dom_id, 
							const char* login );
				    virtual error* user_pass( id_type dom_id, 
							const char* login, const char* pass );
				    virtual error* user_val( const char* user );
				    virtual error* user_get( user_info& ai );
				    virtual error* user_ex( id_type dom_id, 
							const char* login );
				    virtual error* user_ls_by_dom( id_type dom_id, size_type start, size_type cnt, user_info_list_out uis);
					virtual error* user_cnt_by_dom( id_type dom_id, size_type& cnt);

					virtual error* da_add( id_type dom_id, const char* ali );
					virtual error* da_rm( const char* ali );
					virtual error* da_ls_by_dom( id_type dom_id, 
							string_list_out alis );

					virtual error* dip_add( id_type dom_id, const char* ip );
					virtual error* dip_rm( const char* ip );
					virtual error* dip_ls_by_dom( id_type dom_id, 
							string_list_out ips );

					virtual error* eb_add( const char* re_domain, 
							const char* re_login );
					virtual error* eb_rm( const char* re_domain, 
							const char* re_login );
					virtual error* eb_ls( email_banned_list_out ebs );

					virtual error* user_conf_add( id_type dom_id, 
							const char* user, const char* pfix, 
							user_conf_info& ui );
				    virtual error* user_conf_rm_by_type( id_type dom_id, 
							const char* user, const char* pfix, 
							user_conf_type ut );
				    virtual error* user_conf_rm( id_type dom_id, 
							const char* user, const char* pfix, 
							id_type id );
				    virtual error* user_conf_ls( id_type dom_id, 
							const char* user, const char* pfix, 
							user_conf_info_list_out uis );
				    virtual error* user_conf_ls_by_type( id_type dom_id, 
							const char* user, const char* pfix, 
							user_conf_type ut, user_conf_info_list_out uis );
				    virtual error* user_conf_rep( id_type dom_id, 
							const char* user, const char* pfix, 
							const user_conf_info& ui );
				    virtual error* user_conf_get( user_conf_info& ui );
				    virtual error* user_conf_type_has( id_type dom_id, 
							const char* user, const char* pfix, 
							user_conf_type ut );
				    virtual error* user_conf_type_cnt( id_type dom_id, 
							const char* user, const char* pfix, 
							user_conf_type ut, size_type_out cnt );

					inline bool shutdown() const;
			
			protected:
					static const char tmp_end[]; //!< temporary file extension
					
					service_conf conf;
					
					::vq::iauth_var auth; //!< authorization module
					CORBA::Object_var iaobj;
					CORBA::ORB_var orb;
					bool shutting; //!< returned by shutdown

					virtual error* dip_rm_by_dom( id_type dom_id );
					virtual error* da_rm_by_dom( id_type dom_id );

					void auth_init(); //!< used to init auth attribute

					error * send_restart();
					error * assign_ex(const std::string &);
					error * assign_add(const std::string &);
					error * assign_rm(const std::string &);
					std::string assign_ln(const std::string &) const;
					error * rcpt_add(const std::string &);
					error * rcpt_rm(const std::string &);
					error * morercpt_add(const std::string &);
					error * morercpt_rm(const std::string &);
					error * moreipme_add( const std::string & );
					error * moreipme_rm( const std::string & );
					error * locals_rm(const std::string &);
					error * virt_add(const std::string &,const std::string &);
					error * virt_rm(const std::string &);
					error * qmail_file_rm( qmail_file, const std::string & );
					error * qmail_file_add( qmail_file, const std::string & );
					std::string virt_prefix(const std::string &) const;
				
					int run( char * const [] );
				
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
	
				/**
				 * \ingroup user_conf
				 */
				//@{
					/// create path to dot-qmail
					std::string dotfile(const std::string &, const std::string &, const std::string &);
					/// create line which will be added to dot-qmail
					error * user_conf_ln(const user_conf_info &, std::string &);
				//@}
	
				/**
				 * \ingroup dom
				 */
				//@{
					error * dom_del_lock(const std::string &);
					error * dom_del_unlock(const std::string &); 
				//@}

				/**
				 *
				 */
				//@{
					void user_info_fill_empty( user_info & ui );
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

	/**
	 * @return true if this object should be shutdown
	 */
	inline bool cqmailvq::shutdown() const {
		return this->shutting;
	}
	
} // namespace vq

#endif
