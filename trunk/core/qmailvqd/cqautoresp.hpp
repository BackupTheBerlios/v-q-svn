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

#ifndef __CQAUTORESP_H
#define __CQAUTORESP_H

#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <regex.h>

#include "lower.h"
#include "cautoresp.h"

class cqautoresp : public cautoresp {
		public:
			/**
			 * \defgroup err Errors handling
			 */
			/**
			 * \defgroup exceptions Exceptions
			 * \ingroup err
			 */
			//@{
			
				/**
				 * Throw when some environment variable is not set, or
				 * has invalid value
				 */
				class env_miss : public std::runtime_error {
						public:
								env_miss(const std::string & var) 
										: std::runtime_error((std::string)"environment variable is missing: "+var) {}
				};

				/**
				 * This exception means that regular expression could
				 * not have been compiled
				 */
				class regex_comp : public std::runtime_error {
						public:
								regex_comp(const std::string &re)
										: std::runtime_error((std::string)"regcomp failed for: "+re) {}
				};

				/**
				 * Exception means that there were problems reading file
				 */
				class file_read : public std::runtime_error {
						public:
								file_read(const std::string &fn)
										: std::runtime_error((std::string)"error reading: "+fn) {}
				};

				/**
				 * Exception means that there were problems writing file
				 */
				class file_write : public std::runtime_error {
						public:
								file_write(const std::string &fn)
										: std::runtime_error((std::string)"error writing: "+fn) {}
				};

				/**
				 * Some runtime error.
				 */
				class runtime_error : public std::runtime_error {
						public:
								runtime_error(const std::string &s)
										: std::runtime_error(s) {}
				};
			//@}
			
				void msg_set( const std::string & );
				void hdrs_add_set( const map_hdr_val & );
				void reply( const map_hdr_val & );
				void codepage_set( const std::string & );
				cqautoresp();
				virtual ~cqautoresp();

		protected:
				std::string msg, codepage;
				map_hdr_val hdrs_add;
				map_hdr_val hdrs_in;
				regex_t re_x_remark;
				regex_t re_x_mailer;

				std::string subdir(const std::string &, const std::string &);
				static std::string uniq();
				static bool histAdd(const std::string &, const std::string &);
				static bool histHas(const std::string &, const std::string &);
				static bool histIDMatches(const std::string &, 
						const std::string &, const std::string & );
				void msgSend();
				void msgWrite( std::ostream & );

};

#endif
