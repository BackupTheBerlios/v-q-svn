/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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

#ifndef __CAUTH_H
#define __CAUTH_H

#include <string>
#include <vector>

#include "cvq.h"

/**
 * Base class representing authorization module.
 */
class cauth {
		public:
				typedef cvq::size_type size_type;

				enum ret {
						err_temp = 0,
						user_nf, // not found
						user_ok // valid user
				};
				virtual bool dom_add(const std::string &)=0;
				virtual bool dom_ip_add(const std::string &,const std::string &)=0;
				virtual bool dom_ip_rm(const std::string &,const std::string &)=0;
				virtual bool dom_ip_rm_all(const std::string &)=0;
				virtual bool dom_ip_ls(const std::string &,std::vector<std::string> &)=0;
				virtual bool dom_ip_ls_dom(std::vector<std::string> &)=0;
				virtual bool dom_rm(const std::string &)=0;
				
				virtual uint8_t user_add(const std::string &, const std::string &, 
						const std::string &, uint8_t = 0 )=0;
				virtual bool user_rm(const std::string &,const std::string &)=0;
				virtual const char * err()=0;
				virtual bool user_pass(const std::string&,const std::string&,
						const std::string&)=0;
				virtual char user_auth(cvq::auth_info &)=0;
				virtual uint8_t user_ex(const std::string &, const std::string &)=0;
				
				virtual bool udot_add(const std::string &, const std::string &,
								const std::string &, cvq::udot_info & )=0;
				virtual bool udot_rm(const std::string &, const std::string &)=0;
				virtual bool udot_rm(const std::string &, const std::string &,
								const std::string &, cvq::udot_type )=0;
				virtual bool udot_ls(const std::string &, const std::string &,
								const std::string &, std::vector<cvq::udot_info> & )=0;
				virtual bool udot_ls(const std::string &, const std::string &,
								const std::string &, cvq::udot_type ,
								std::vector<cvq::udot_info> & )=0;
				virtual bool udot_rep(const std::string &, const cvq::udot_info & )=0;
				virtual bool udot_get(const std::string &,
								cvq::udot_info & )=0;
				virtual bool udot_has(const std::string &, const std::string &,
								const std::string &, cvq::udot_type, bool = true )=0;
				virtual bool udot_type_cnt(const std::string &, const std::string &,
								const std::string &, cvq::udot_type, cvq::size_type & )=0;
};

#endif
