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
