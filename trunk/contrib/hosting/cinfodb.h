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

#ifndef __CINFODB_H
#define __CINFODB_H

#include <inttypes.h>
#include <string>
#include <vector>

using namespace std;

/**
 * Access to database.
 */
class cinfodb {
		public:
			/**
			 * \ingroup err
			 */
			//@{
				/// Codes returned by functions
				enum error {
					err_no = 0, //!< no error
					err_read, //!< can't read from database
					err_write, //!< can't write to database
					err_noent, //!< entry not found
				};
			//@}

			/**
			 * \ingroup wwwali
			 */
			//@{
				/// Information about WWW alias.
				typedef struct {
					std::string id; // id.
					std::string dir; // directory
					std::string prefix; // prefix (www, ftp)
				} wwwali_info;

				/// Information about domain
				typedef struct {
					std::string id;
					std::string domain;
				} wwwali_dom_info;
			//@}

			/**
			 * \ingroup user
			 */
			//@{
				/// Information about user
				typedef struct {
					std::string id; //!< entry's id
					std::string user; //!< user
					std::string date_crt; //!< account's creation date
					std::string date_exp; //!< expire date
				} user_info;
			//@}

			/**
			 * \ingroup ftp
			 */
			//@{
				/// Information about FTP access.
				typedef struct {
					std::string id; //!< entry's id
					std::string user; //!< user
					std::string dir; //!< subdirectory
				} ftp_info;
			//@}

			/**
			 * \defgroup wwwali WWW aliases
			 */
			//@{
				virtual uint8_t wwwali_ls(const std::string &, 
						std::vector<wwwali_dom_info> &)=0;
				virtual uint8_t wwwali_dom_ls(const std::string &, 
						std::vector<wwwali_info> & )=0;
				virtual uint8_t wwwali_dom_ls_dir(const std::string &, 
						const std::string &, std::vector<wwwali_info> & )=0;
				virtual uint8_t wwwali_dom_id_get(const std::string &, 
						std::string &)=0;
				virtual uint8_t wwwali_rep(const std::string &, 
						const wwwali_info & )=0;
				virtual uint8_t wwwali_rm(const std::string &, 
						const std::string &)=0;
				virtual uint8_t wwwali_add(const std::string &, wwwali_info &)=0;
				virtual uint8_t wwwali_get(const std::string &, wwwali_info &)=0;
				virtual uint8_t wwwali_dom_get(const std::string &, 
						std::string &)=0;
				virtual uint8_t wwwali_rdom_get(const std::string &, 
						std::string &)=0;
			//@}

			/**
			 * \defgroup user Users
			 */
			//@{
				virtual uint8_t user_ls(const std::string &, 
						std::vector<user_info> & )=0;

				virtual uint8_t ui_id_get(const std::string &, 
						const std::string &, std::string &)=0;
			//@}

			/**
			 * \defgroup ftp FTP access
			 */
			//@{
				/// adds user which can log into some part of ftp server
				virtual uint8_t ftp_add(const std::string &, const ftp_info &)=0;
				virtual uint8_t ftp_rm(const std::string &, 
						const std::string &)=0;
				virtual uint8_t ftp_ls(const std::string &, 
						std::vector<ftp_info> & )=0;
				virtual uint8_t ftp_rep(const std::string &, const ftp_info &)=0;
				/// get ftp_info for specified user
				virtual uint8_t ftp_get(const std::string &, ftp_info &)=0;
			//@}

				static cinfodb *alloc(); //!< allocate implementation object
};

#endif
