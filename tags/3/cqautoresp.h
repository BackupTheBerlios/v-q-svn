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
				class env_miss : public std::runtime_error {
						public:
								env_miss(const std::string & var) 
										: std::runtime_error((std::string)"environment variable is missing: "+var) {}
				};

				class regex_comp : public std::runtime_error {
						public:
								regex_comp(const std::string &re)
										: std::runtime_error((std::string)"regcomp failed for: "+re) {}
				};

				class file_read : public std::runtime_error {
						public:
								file_read(const std::string &fn)
										: std::runtime_error((std::string)"error reading: "+fn) {}
				};

				class file_write : public std::runtime_error {
						public:
								file_write(const std::string &fn)
										: std::runtime_error((std::string)"error writing: "+fn) {}
				};

				class runtime_error : public std::runtime_error {
						public:
								runtime_error(const std::string &s)
										: std::runtime_error(s) {}
				};
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
		public:
				void msg_set( const std::string & );
				void hdrs_add_set( const map_hdr_val & );
				void reply( const map_hdr_val & );
				void codepage_set( const std::string & );
				cqautoresp();
				virtual ~cqautoresp();
};

#endif
