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

#ifndef __CFSVQ_H
#define __CFSVQ_H

#include <stdexcept>
#include <string>
#include <fstream>

#include "cvq.h"
#include "cauth.h"

/**
 * Implementation of cvq. It stores everything on filesystem (in directory
 * conf_vq/domains/).
 */
class cfsvq : public cvq {
        protected:
                static const char tmp_end[];
                        
                uint8_t lastret;
                cauth *auth;
                struct { 
                        std::string assign_add, rcpt_add, morercpt_add, locals_rm;
                        std::string virt_add, dom_add_dir, virt_rm, rcpt_rm;
                        std::string morercpt_rm, assign_rm;
                } tmpfs;
                /// array of supported udot types
                static char ud_sup[];

                char assign_ex(const std::string &) throw();
                char assign_add(const std::string &) throw();
                char assign_rm(const std::string &) throw();
                char rcpt_add(const std::string &) throw();
                char rcpt_rm(const std::string &) throw();
                char morercpt_add(const std::string &) throw();
                char morercpt_rm(const std::string &) throw();
                char morercpt_comp() throw();
                char assign_comp() throw();
                char locals_rm(const std::string &) throw();
                char virt_add(const std::string &,const std::string &) throw();
                char virt_rm(const std::string &) throw();
                char maildirmake(const std::string &) throw();

                /// some uniqe value
                std::string uniq() throw(std::runtime_error);
                void dom_add_clean(const std::string &) throw();
                

                
                uint8_t qtf_cur_set(const std::string &, int32_t) throw();
                uint8_t qtf_set(const std::string &, quota_value) throw();
                uint8_t qtf_set(const std::string &, quota_value, quota_value) throw();
                uint8_t qtf_over(const std::string &) throw();
                uint8_t qtf_get(const std::string &, quota_value &, quota_value &) throw();

                void assert_auth() throw(std::logic_error);
                
                // dot files support
                std::string dotfile(const std::string &, const std::string &, const std::string &) throw();
                std::string udot_ln(const udot_info &) throw();

                std::string dompath(const std::string &) throw();

                uint8_t dom_del_lock(const std::string &) throw();
                uint8_t dom_del_unlock(const std::string &) throw(); 
                
                uint8_t tmp_crt(const std::string &, std::ofstream &, std::string & ) throw();
        public:
                cfsvq(cauth *);
                
                char dom_add(const std::string &) throw();
                char dom_ip_add(const std::string &,const std::string &) throw();
                char dom_ip_rm(const std::string &,const std::string &) throw();
                char dom_ip_rm_all(const std::string &) throw();
                char dom_ip_ls(const std::string &,std::vector<std::string> &) throw();
                char dom_ip_ls_dom(std::vector<std::string> &) throw();
                char dom_rm(const std::string &) throw();
                virtual ~cfsvq(){}
                const char * err() { 
                        return auth ? ((std::string)err_str(lastret)+": AUTH: "+auth->err()).c_str() : err_str(lastret); 
                }
                char user_add(const std::string &, const std::string &, 
                                const std::string &, uint8_t = 0 ) throw();
                char user_pass(const std::string &,const std::string &,const std::string &)
                        throw();
                char user_rm(const std::string &,const std::string &) throw();
                char dom_alias_add(const std::string &,const std::string &) throw();
                char dom_alias_rm(const std::string &) throw();
                char dom_val(const std::string &) throw();
                char user_val(const std::string &) throw(); 
                char user_auth(auth_info &) throw();
                uint8_t user_ex(const std::string &, const std::string &) throw();

                /// quota support
                std::string qtfile(const std::string &, const std::string &) throw();
                std::string qtfile(const std::string &) throw();
                std::string qtfile_def(const std::string &) throw();
                std::string qtfile_def() throw();
                
                uint8_t qt_cur_set(const std::string&, const std::string &, int32_t) throw();
                uint8_t qt_set(const std::string &, const std::string &, quota_value ) throw();
                uint8_t qt_set(const std::string &, const std::string &, quota_value, quota_value ) throw();
                uint8_t qt_get(const std::string&, const std::string &, quota_value &, quota_value &) throw();
                uint8_t qt_over(const std::string &, const std::string &) throw();
                uint8_t qt_def_set(const std::string &, quota_value) throw();
                uint8_t qt_def_get(const std::string &, quota_value &) throw();
                uint8_t qt_def_cp(const std::string &, const std::string &) throw();
                
                uint8_t qt_dom_cur_set(const std::string &, int32_t) throw();
                uint8_t qt_dom_set(const std::string &, quota_value) throw();
                uint8_t qt_dom_set(const std::string &, quota_value, quota_value) throw();
                uint8_t qt_dom_over(const std::string &) throw();
                uint8_t qt_dom_get(const std::string &, quota_value &, quota_value &) throw();
                uint8_t qt_dom_def_set(quota_value) throw();
                uint8_t qt_dom_def_get(quota_value &) throw();
                uint8_t qt_dom_def_cp(const std::string &) throw();

                uint8_t udot_add(const std::string &, const std::string &,
                                const std::string &, udot_info & ) throw();
                uint8_t udot_rm(const std::string &, const std::string &,
                                const std::string &, const std::string &) throw(std::logic_error);
                uint8_t udot_ls(const std::string &, const std::string &,
                                const std::string &, std::vector<udot_info> & ) throw();
                uint8_t udot_ls(const std::string &, const std::string &,
                                const std::string &, char,
                                std::vector<udot_info> & ) throw();
                uint8_t udot_rep(const std::string &, const std::string &, 
                                const std::string &, const udot_info & ) throw();
                uint8_t udot_get(const std::string &, udot_info &) throw(std::logic_error);
                uint8_t udot_sup_is(char) throw ();
                uint8_t udot_has(const std::string &, const std::string &,
                                const std::string &, char );
                uint8_t udot_add_md_def(const std::string &, const std::string &,
                                const std::string &);
                uint8_t udot_rm(const std::string &, const std::string &,
                                const std::string &, char );
                uint8_t udot_type_cnt(const std::string &, const std::string &,
                                const std::string &, char, size_type & ) throw();
                std::string udot_sup() throw ();
};

#endif
