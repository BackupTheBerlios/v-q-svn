/*
Copyright (c) 2003 Pawel Niewiadomski
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
