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

#ifndef __CDBDAEMON_H
#define __CDBDAEMON_H

#include <stdexcept>
#include <sys/types.h>
#include <sys/un.h>
#include <string>
#include <inttypes.h>
#include <vector>

/**
 * This class is used to access database (via special daemon).
 */
class cdbdaemon {
        protected:
                char resp, cmd;
                int cso;
                struct sockaddr_un cun;
                std::string errmsg, clip, loc;
               
        public:
			/**
			 * \ingroup exceptions
			 */
			//@{

				/// Out runtime error
                class rt_error : public std::runtime_error {
                        public:
                                rt_error( const std::string & );
                };
			//@}

			/**
			 * \defgroup ui Informations about user
			 */
			//@{

				/// Education level
                struct edu_info {
                        std::string id, name;
                };

				/// Hobbies.
                struct int_info {
                        std::string id, name;
                };

				/// Current work
                struct work_info {
                        std::string id, name;
                };

				/// Part of country
                struct area_info {
                        std::string id, name;
                };

				/// Sex
                struct sex_info {
                        std::string id, name;
                };

				/// All informations about user
                struct user_info {
                        std::string id, login, dom_id, country, city;
                        std::string birthday, area_id, work_id, edu_id;
                        std::string sex_id, locale;
                        std::vector<std::string> ints_id;
                        std::string re_que, re_ans;
                };

				/// Information about domain (from e-mail)
                struct domain_info {
                        std::string id, name;
                };

				/// All informations + names
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
			//@}

                cdbdaemon();
                virtual ~cdbdaemon();
                
                /// Commands send to daemon
                enum cmd {
                        cmd_edu_ls=0, //!< get education levels
                        cmd_work_ls, //!< get works
                        cmd_int_ls, //!< get hobbies
                        cmd_area_ls, //!< list areaas
                        cmd_sex_ls, //!< list sexes
                        cmd_dom_ls, //!< list domains
                        cmd_ui_add, //!< add informations about user
                        cmd_ui_rm, //!< remove informations about user
                        cmd_dom_get, //!< get domain
                        cmd_dom_get_by_name, //!< get domain by name
                        cmd_ui_get_by_name, //!< get info. by name
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

			/**
			 * \ingroup err
			 */
			//@{
                /// error codes
                enum error {
                        err_no = 0, //!< no error
                        err_empty, //!< empty query
                        err_write, //!< can't write to daemon
                        err_read //!< can't read from daemon
                };
                
                /// Return error desc.
                const char *err() const { return errmsg.c_str(); }
			//@}

                /// set locale (data may vary for locale)
                void locale_set( const std::string & );

                /// get education levels
                uint8_t edu_ls( std::vector<edu_info> & );

                /// get works
                uint8_t work_ls( std::vector<work_info> & );

                /// get hobbies (interests)
                uint8_t int_ls( std::vector<int_info> & );

                /// get areas 
                uint8_t area_ls( std::vector<area_info> & );

                /// sesex list
                uint8_t sex_ls( std::vector<sex_info> & );

                /// domains list
                uint8_t domain_ls( std::vector<domain_info> & );
                
                /// get domain's name by id
                uint8_t domain_get( domain_info & );
                
                ///
                uint8_t domain_get_by_name( domain_info & );

                /// user's info add
                uint8_t ui_add( user_info & );

                /// change user's info
                uint8_t ui_chg( const user_info & );

                /// remove user's info
                uint8_t ui_rm( const std::string & );

                ///
                uint8_t ui_get_by_name( ui_full & );
                
                ///
                uint8_t ui_get_by_name( user_info & );

                /// compares user information with copy stored in database
                uint8_t ui_cmp_by_name( const user_info & );

                /// get rescue question
                uint8_t ui_re_que_get_by_name( const std::string &, const std::string &,
                                std::string & );

                /// 
                uint8_t user_pass_get( const std::string &, const std::string &,
                                std::string  & );

				/// allocate implementation object
				static cdbdaemon * alloc();
};

#endif
