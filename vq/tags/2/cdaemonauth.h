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

#ifndef __CDAEMONAUTH_H
#define __CDAEMONAUTH_H

#include <string>
#include <stdexcept>
#include <vector>
#include <sys/types.h>
#include <sys/un.h>

#include "cauth.h"

/**
 * Daemon implementation of cauth class. It connects to daemon via unix
 * socket, it sends all command to it. Daemon can store informations
 * in database or filesystem.
 */
class cdaemonauth : public cauth {
        public:
                enum cmd {
                        cmd_dom_add = 0,
                        cmd_dom_rm,
                        cmd_user_add,
                        cmd_user_rm,
                        cmd_user_pass,
                        cmd_user_auth,
                        cmd_dom_ip_add,
                        cmd_dom_ip_rm,
                        cmd_dom_ip_rm_all,
                        cmd_dom_ip_ls,
                        cmd_dom_ip_ls_dom,
                        cmd_udot_add,
                        cmd_udot_rm,
                        cmd_udot_ls,
                        cmd_udot_ls_type,
                        cmd_udot_rep,
                        cmd_udot_get,
                        cmd_udot_has,
                        cmd_udot_rm_type,
                        cmd_udot_type_cnt,
                        cmd_user_ex
                };

        protected:
                char cmd, resp;
                int cso;
                struct sockaddr_un cun;
                std::string errmsg;
        public:
                cdaemonauth() throw(std::runtime_error);
                bool dom_add(const std::string &) throw();
                bool dom_ip_add(const std::string&,const std::string &) throw();
                bool dom_ip_rm(const std::string&,const std::string &) throw();
                bool dom_ip_ls(const std::string&,std::vector<std::string> &) throw();
                bool dom_ip_ls_dom(std::vector<std::string> &) throw();
                bool dom_ip_rm_all(const std::string&) throw();
                bool dom_rm(const std::string &) throw();

                uint8_t user_add(const std::string &, const std::string &, 
                                const std::string &, uint8_t = 0 ) throw();
                bool user_rm(const std::string &,const std::string &) throw();
                const char * err() throw() { return errmsg.c_str(); }
                virtual ~cdaemonauth() throw();
                bool user_pass(const std::string&,const std::string&, const std::string&)
                        throw();
                char user_auth(cvq::auth_info &) throw();
                uint8_t user_ex(const std::string &, const std::string &) throw();
                
                bool udot_add(const std::string &, const std::string &,
                                const std::string &, cvq::udot_info & ) throw();
                bool udot_rm(const std::string &, const std::string &) throw();
                bool udot_rm(const std::string &, const std::string &,
                                const std::string &, char ) throw();
                bool udot_ls(const std::string &, const std::string &,
                                const std::string &, std::vector<cvq::udot_info> & ) throw();
                bool udot_ls(const std::string &, const std::string &,
                                const std::string &, char,
                                std::vector<cvq::udot_info> & ) throw();
                bool udot_rep(const std::string &, const cvq::udot_info & ) throw();
                bool udot_get(const std::string &,
                                cvq::udot_info & ) throw();
                bool udot_has(const std::string &, const std::string &, const std::string &,
                                char, bool = true ) throw();
                bool udot_type_cnt(const std::string &, const std::string &, 
                                const std::string &, char , cvq::size_type & ) throw();
};

#endif
