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

#ifndef __CENVIRONMENT_H
#define __CENVIRONMENT_H

#include <iostream>
#include <vector>
#include <deque>
#include <memory>
#include <sstream>

// Cgicc
#include <cgicc/Cgicc.h>

// VQ
#include <cvq.h>
#include <clogger.h>

#include "csess.h"
#include "vqwww.h"
#include "sabmain.h"

class cmod;

/**
 * Module's environment
 */
class cenvironment {
		public:
				enum request_method {
					rm_post,
					rm_get
				};

				typedef std::vector<cgicc::HTTPCookie> cookies_set;
				typedef std::map<std::string, cgicc::HTTPCookie> cookies_map;

		protected:
				const int ac;
				const char ** av;
				
				static bool instance;
				
				std::auto_ptr<cvq> vq;
				std::auto_ptr<csess> sess;
				std::auto_ptr<cgicc::Cgicc> cgi;
				std::auto_ptr<clogger> logger;
				csess_basic sessb;
				cookies_map cgi_cks_set;
				request_method cgi_rm;
				cvqwww_dirs dirs;
				std::ostringstream mod_out;
				std::ostream &err; //!< error reporting stream
				std::ostream &out; //!< output stream
				std::string locale;
						
				cenvironment(int, const char **, std::ostream &, std::ostream &,
						xslt_args_type & );
				cenvironment & operator = ( const cenvironment & );

		public:			
				std::deque< cmod * > mods;
				xslt_args_type &xslt_args;

			/**
			 * \defgroup args Arguments
			 */
			//@{
				const char ** av_get() const {
					return av;
				}

				int ac_get() const {
					return ac;
				}
			//@}
			
			/**
			 * \defgroup outstr Output streams
			 */
			//@{
				std::ostream & err_get() {
					return err;
				}

				std::ostream & out_get() {
					return mod_out;
				}
			//@}
			
				cvq & vq_get() {
					return *vq;
				}

			/**
			 * \defgroup sess Session
			 */
			//@{
				csess & sess_get() { //!< session
					return *sess;
				}

				/// basic variables stored in session
				const csess_basic & sessb_get() const {
					return sessb;
				}
			//@}
			
			/**
			 * \defgroup cgi CGI
			 */
			//@{
				cgicc::Cgicc & cgi_get() {
					return *cgi;
				}

				const cgicc::Cgicc & cgi_get() const {
					return *cgi;
				}
				
				const cgicc::CgiEnvironment & cgi_env_get() const {
					return cgi->getEnvironment();
				}
				
				cgicc::const_form_iterator cgi_end_get() const {
					return cgi_get().getElements().end();
				}

				const cookies_set & cgi_cks_get() const {
					return cgi_env_get().getCookieList();
				}
				
				cookies_map & cgi_cks_set_get() {
					return cgi_cks_set;
				}
				
				request_method cgi_rm_get() {
					return cgi_rm;
				}
			//@}

				const cvqwww_dirs & dirs_get() const {
					return dirs;
				}

				clogger & logger_get() {
					return *logger;
				}

				/**
				 *
				 */
				const std::string & locale_get() const {
					return locale;
				}
};

#endif // ifndef __CENVIRONMENT_H
