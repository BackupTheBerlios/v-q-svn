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

#ifndef __CDBDAEMON_H
#define __CDBDAEMON_H

#include <stdexcept>
#include <sys/types.h>
#include <sys/un.h>
#include <string>
#include <inttypes.h>
#include <vector>

/**
 */
class cdbdaemon {
        protected:
                char resp, cmd;
                int cso;
                struct sockaddr_un cun;
                std::string errmsg, clip, loc;
               
        public:
                class rt_error : public std::runtime_error {
                        public:
                                rt_error( const std::string & );
                };

                struct edu_info {
                        std::string id, name;
                };

                struct int_info {
                        std::string id, name;
                };

                struct work_info {
                        std::string id, name;
                };

                struct area_info {
                        std::string id, name;
                };

                struct sex_info {
                        std::string id, name;
                };

                struct user_info {
                        std::string id, login, dom_id, country, city;
                        std::string birthday, area_id, work_id, edu_id;
                        std::string sex_id, locale;
                        std::vector<std::string> ints_id;
                        std::string re_que, re_ans;
                };

                struct ui_full {
                        std::string id, login, dom_id, country, city;
                        std::string birthday, re_que, re_ans;
                        std::string locale;
                        area_info area;
                        work_info work;
                        edu_info edu;
                        sex_info sex;
                        std::vector< int_info > ints;
                };

                struct domain_info {
                        std::string id, name;
                };
                
                cdbdaemon() throw(rt_error);
                virtual ~cdbdaemon();
                
                /// commands
                enum cmd {
                        cmd_edu_ls=0, //!< get education levels
                        cmd_work_ls, //!< get works
                        cmd_int_ls, //!< get hobbies
                        cmd_area_ls, //!< list areaas
                        cmd_sex_ls,
                        cmd_dom_ls,
                        cmd_ui_add,
                        cmd_ui_rm,
                        cmd_dom_get,
                        cmd_dom_get_by_name,
                        cmd_ui_get_by_name,
                        cmd_ui_work_get,
                        cmd_ui_area_get,
                        cmd_ui_sex_get,
                        cmd_ui_edu_get,
                        cmd_ui_ints_get,
                        cmd_ui_cmp_by_name,
                        cmd_user_pass_get,
                        cmd_ui_re_que_get_by_name,
                        cmd_ui_get_by_name1,
                        cmd_ui_chg
                };

                /// error codes
                enum err {
                        err_no = 0, //!< no error
                        err_empty, //!< empty query
                        err_write, //!< can't write to daemon
                        err_read //!< can't read from daemon
                };
                
                ///
                const char *err() const { return errmsg.c_str(); }

                /// set locale (data may vary for locale)
                void locale_set( const std::string & );

                /// get education levels
                uint8_t edu_ls( std::vector<edu_info> & ) throw();

                /// get works
                uint8_t work_ls( std::vector<work_info> & ) throw();

                /// get hobbies (interests)
                uint8_t int_ls( std::vector<int_info> & ) throw();

                /// get areas 
                uint8_t area_ls( std::vector<area_info> & ) throw();

                /// sesex list
                uint8_t sex_ls( std::vector<sex_info> & ) throw();

                /// domains list
                uint8_t domain_ls( std::vector<domain_info> & ) throw();
                
                /// get domain's name by id
                uint8_t domain_get( domain_info & ) throw();
                
                ///
                uint8_t domain_get_by_name( domain_info & ) throw();

                /// user's info add
                uint8_t ui_add( user_info & ) throw();

                /// change user's info
                uint8_t ui_chg( const user_info & ) throw();

                /// remove user's info
                uint8_t ui_rm( const std::string & ) throw();

                ///
                uint8_t ui_get_by_name( ui_full & ) throw();
                
                ///
                uint8_t ui_get_by_name( user_info & ) throw();

                /// compares user information with copy stored in database
                uint8_t ui_cmp_by_name( const user_info & ) throw();

                /// get rescue question
                uint8_t ui_re_que_get_by_name( const std::string &, const std::string &,
                                std::string & ) throw();

                /// 
                uint8_t user_pass_get( const std::string &, const std::string &,
                                std::string  & ) throw();
};

void db_init( cdbdaemon ** ) throw(std::bad_alloc,cdbdaemon::rt_error);

#endif
