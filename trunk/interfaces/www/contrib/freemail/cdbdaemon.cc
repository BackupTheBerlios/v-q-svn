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

#include <new>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <fd.h>
#include <fdstr.h>
#include <lower.h>

#include "cdbdaemon.h"
#include "conf_home.h"

using namespace std;
using namespace freemail;

/**
 *
 */
cdbdaemon * cdbdaemon::alloc() {
	return new cdbdaemon();
}

cdbdaemon::rt_error::rt_error( const string & w )
        : runtime_error( w ) {
}

/**
 *
 */
cdbdaemon::cdbdaemon() {
    int curdirfd = open(".",O_RDONLY);
    if(curdirfd==-1) 
            throw rt_error((string)"open: .: "+strerror(errno));
    
    string dir_dest = conf_home+"/var/run/vqsignup1d";
    if(chdir(dir_dest.c_str()))
            throw rt_error((string)"chdir: "+dir_dest+": "+strerror(errno));

    if( (cso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
            throw rt_error((string)"socket: "+strerror(errno));

    string sck_name = "vqsignup1d.sock";
    cun.sun_family = AF_UNIX;
    strcpy(cun.sun_path, sck_name.c_str());
    if(connect(cso, (struct sockaddr*) &cun, SUN_LEN(&cun)))
            throw rt_error((string)"connect: "+sck_name+": "+strerror(errno));
    if( fchdir(curdirfd) )
            throw rt_error((string)"fchdir: "+conf_home+": "+strerror(errno));
	close(curdirfd);
}

cdbdaemon::~cdbdaemon() {
    close(cso);
}

/**
 *
 */
uint8_t cdbdaemon::edu_ls( vector<edu_info> & vec ) {
 resp = 0;
 cmd = cmd_edu_ls;
 vector<edu_info>::size_type cnt;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   vec.reserve(cnt);
   edu_info info;
   cnt--;
   do {
     if( fdrdstr(cso, info.id) == -1 
         || fdrdstr(cso, info.name) == -1 ) return err_read;
     vec.push_back(info);
   } while(cnt--);
   return err_no;
 }
 if(resp == 'Z') return err_empty; 
 return resp ? err_read : err_write;
}

/**
 *
 */
uint8_t cdbdaemon::int_ls( vector<int_info> & vec ) {
 resp = 0;
 cmd = cmd_int_ls;
 vector<int_info>::size_type cnt;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   vec.reserve(cnt);
   int_info info;
   cnt--;
   do {
     if( fdrdstr(cso, info.id) == -1 
         || fdrdstr(cso, info.name) == -1 ) return err_read;
     vec.push_back(info);
   } while(cnt--);
   return err_no;
 }
 if(resp == 'Z') return err_empty; 
 return resp ? err_read : err_write;
}

/**
 *
 */
uint8_t cdbdaemon::work_ls( vector<work_info> & vec ) {
 resp = 0;
 cmd = cmd_work_ls;
 vector<work_info>::size_type cnt;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   vec.reserve(cnt);
   work_info info;
   cnt--;
   do {
     if( fdrdstr(cso, info.id) == -1 
         || fdrdstr(cso, info.name) == -1 ) return err_read;
     vec.push_back(info);
   } while(cnt--);
   return err_no;
 }
 if(resp == 'Z') return err_empty; 
 return resp ? err_read : err_write;
}

/**
 *
 */
void cdbdaemon::locale_set( const std::string & l ) {
 loc = l;
}

/**
 *
 */
uint8_t cdbdaemon::area_ls( vector<area_info> & vec ) {
 resp = 0;
 cmd = cmd_area_ls;
 vector<area_info>::size_type cnt;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   vec.reserve(cnt);
   area_info info;
   cnt--;
   do {
     if( fdrdstr(cso, info.id) == -1 
         || fdrdstr(cso, info.name) == -1 ) return err_read;
     vec.push_back(info);
   } while(cnt--);
   return err_no;
 }
 if(resp == 'Z') return err_empty; 
 return resp ? err_read : err_write;
}

/**
 *
 */
uint8_t cdbdaemon::sex_ls( vector<sex_info> & vec ) {
 resp = 0;
 cmd = cmd_sex_ls;
 vector<sex_info>::size_type cnt;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   vec.reserve(cnt);
   sex_info info;
   cnt--;
   do {
     if( fdrdstr(cso, info.id) == -1 
         || fdrdstr(cso, info.name) == -1 ) return err_read;
     vec.push_back(info);
   } while(cnt--);
   return err_no;
 }
 if(resp == 'Z') return err_empty; 
 return resp ? err_read : err_write;
}

/**
 *
 */
uint8_t cdbdaemon::domain_ls( vector<domain_info> & vec ) {
 resp = 0;
 cmd = cmd_dom_ls;
 vector<domain_info>::size_type cnt;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   vec.reserve(cnt);
   domain_info info;
   cnt--;
   do {
     if( fdrdstr(cso, info.id) == -1 
         || fdrdstr(cso, info.name) == -1 ) return err_read;
     vec.push_back(info);
   } while(cnt--);
   return err_no;
 }
 if(resp == 'Z') return err_empty; 
 return resp ? err_read : err_write;
}

/**
 *
 */
uint8_t cdbdaemon::ui_get_by_name( ui_full & ui ) {
 vector<int_info>::size_type cnt, i;
 int_info inter;
 resp = 0;
 cmd = cmd_ui_get_by_name;
 
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdwrstr(cso, lower(ui.login)) != -1
     && fdwrstr(cso, ui.dom_id) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdrdstr(cso, ui.id) != -1
     && fdrdstr(cso, ui.country) != -1
     && fdrdstr(cso, ui.city) != -1
     && fdrdstr(cso, ui.birthday) != -1
     && fdrdstr(cso, ui.re_que) != -1
     && fdrdstr(cso, ui.re_ans) != -1
     && fdrdstr(cso, ui.locale) != -1
     && fdrdstr(cso, ui.area.id) != -1
     && fdrdstr(cso, ui.area.name) != -1
     && fdrdstr(cso, ui.work.id) != -1
     && fdrdstr(cso, ui.work.name) != -1
     && fdrdstr(cso, ui.edu.id) != -1
     && fdrdstr(cso, ui.edu.name) != -1
     && fdrdstr(cso, ui.sex.id) != -1
     && fdrdstr(cso, ui.sex.name) != -1
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   for( i=0; i < cnt; ++i ) {
     if( fdrdstr(cso, inter.id) == -1 
         || fdrdstr(cso, inter.name) == -1 ) return err_read;
     ui.ints.push_back(inter);
   }
   return err_no;
 } 
 return resp ? err_write : err_read;
}

/**
 *
 */
uint8_t cdbdaemon::ui_get_by_name( user_info & ui ) {
 vector<int_info>::size_type cnt, i;
 string inter;
 resp = 0;
 cmd = cmd_ui_get_by_name1;
 
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdwrstr(cso, lower(ui.login)) != -1
     && fdwrstr(cso, ui.dom_id) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdrdstr(cso, ui.id) != -1
     && fdrdstr(cso, ui.country) != -1
     && fdrdstr(cso, ui.city) != -1
     && fdrdstr(cso, ui.birthday) != -1
     && fdrdstr(cso, ui.re_que) != -1
     && fdrdstr(cso, ui.re_ans) != -1
     && fdrdstr(cso, ui.locale) != -1
     && fdrdstr(cso, ui.area_id) != -1
     && fdrdstr(cso, ui.work_id) != -1
     && fdrdstr(cso, ui.edu_id) != -1
     && fdrdstr(cso, ui.sex_id) != -1
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   for( i=0; i < cnt; ++i ) {
     if( fdrdstr(cso, inter) == -1 ) return err_read;
     ui.ints_id.push_back(inter);
   }
   return err_no;
 } 
 return resp ? err_write : err_read;
}

/**
 *
 */
uint8_t cdbdaemon::ui_add( user_info & ui ) {
 vector<int_info>::size_type cnt = ui.ints_id.size();
 resp = 0;
 cmd = cmd_ui_add;
 
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdwrstr(cso, lower(ui.login)) != -1
     && fdwrstr(cso, ui.dom_id) != -1
     && fdwrstr(cso, ui.country) != -1
     && fdwrstr(cso, ui.city) != -1
     && fdwrstr(cso, ui.birthday) != -1
     && fdwrstr(cso, ui.area_id) != -1
     && fdwrstr(cso, ui.work_id) != -1
     && fdwrstr(cso, ui.edu_id) != -1
     && fdwrstr(cso, ui.sex_id) != -1 
     && fdwrstr(cso, ui.re_que) != -1
     && fdwrstr(cso, ui.re_ans) != -1
     && fdwrite(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   vector<string>::const_iterator icur = ui.ints_id.begin(), iend = ui.ints_id.end();
   for( ; icur!=iend; ++icur )
     if( fdwrstr(cso, *icur) == -1 ) return err_write;
   if( fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
       && resp == 'F'
       && fdrdstr(cso, ui.id) != -1 ) return err_no;
 } 
 return resp ? err_read : err_write;
}

/**
 * change informations about user (don't change login and domain)
 */
uint8_t cdbdaemon::ui_chg( const user_info & ui ) {
 vector<int_info>::size_type cnt = ui.ints_id.size();
 resp = 0;
 cmd = cmd_ui_chg;
 
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdwrstr(cso, lower(ui.id)) != -1
     && fdwrstr(cso, ui.country) != -1
     && fdwrstr(cso, ui.city) != -1
     && fdwrstr(cso, ui.birthday) != -1
     && fdwrstr(cso, ui.area_id) != -1
     && fdwrstr(cso, ui.work_id) != -1
     && fdwrstr(cso, ui.edu_id) != -1
     && fdwrstr(cso, ui.sex_id) != -1 
     && fdwrstr(cso, ui.re_que) != -1
     && fdwrstr(cso, ui.re_ans) != -1
     && fdwrite(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
   
   vector<string>::const_iterator icur = ui.ints_id.begin(), iend = ui.ints_id.end();
   for( ; icur!=iend; ++icur )
     if( fdwrstr(cso, *icur) == -1 ) return err_write;
   if( fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
       && resp == 'K' ) return err_no;
 } 
 return resp ? err_read : err_write;
}


/**
 * Compares given user's information with one stored in database, compared
 * are (cs - case sensitive, ci - case insesitive): locale (cs), 
 * country (ci), city (ci), re_ans (ci), birthday, area_id, work_id, edu_id,
 * sex_id
 * \return err_no - match, err_empty - no match, ... - error
 */
uint8_t cdbdaemon::ui_cmp_by_name( const user_info & ui ) {
 resp = 0;
 cmd = cmd_ui_cmp_by_name;
 
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, loc) != -1 
     && fdwrstr(cso, lower(ui.login)) != -1
     && fdwrstr(cso, ui.dom_id) != -1
     && fdwrstr(cso, ui.country) != -1
     && fdwrstr(cso, ui.city) != -1
     && fdwrstr(cso, ui.birthday) != -1
     && fdwrstr(cso, ui.area_id) != -1
     && fdwrstr(cso, ui.work_id) != -1
     && fdwrstr(cso, ui.edu_id) != -1
     && fdwrstr(cso, ui.sex_id) != -1 
     && fdwrstr(cso, ui.re_ans) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'K' ) return err_no;
 if( 'Z' == resp ) return err_empty;
 return resp ? err_read : err_write;
}


/**
 *
 */
uint8_t cdbdaemon::ui_rm( const string & id ) {
 resp = 0;
 cmd = cmd_ui_rm;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, id) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'K' ) return err_no;
   
 return resp ? err_read : err_write;
}

/**
 *
 */
uint8_t cdbdaemon::domain_get( domain_info & di ) {
 resp = 0;
 cmd = cmd_dom_get;

 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
     && fdwrstr(cso, di.id) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdrdstr(cso, di.name) != -1 ) {
   return err_no;
 }
 if( resp == 'Z' ) return err_empty;
 return resp ? err_read : err_write;
}

/**
 *
 */
uint8_t cdbdaemon::domain_get_by_name( domain_info & di ) {
 resp = 0;
 cmd = cmd_dom_get_by_name;

 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
     && fdwrstr(cso, lower(di.name)) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdrdstr(cso, di.id) != -1 ) {
   return err_no;
 }
 if( resp == 'Z' ) return err_empty;
 return resp ? err_read : err_write;
}

/**
 *
 */
uint8_t cdbdaemon::user_pass_get( const string &dom, const string &user,
                string & pass ) {
 resp = 0;
 cmd = cmd_user_pass_get;

 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd) 
     && fdwrstr(cso, lower(dom)) != -1
     && fdwrstr(cso, lower(user)) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F' 
     && fdrdstr(cso, pass) != -1 ) {
   return err_no;
 }
 if( resp == 'Z' ) return err_empty;
 return resp ? err_read : err_write;
}
/**
 *
 */
uint8_t cdbdaemon::ui_re_que_get_by_name( const string & dom_id, const string & login,
               string &que) {
 resp = 0;
 cmd = cmd_ui_re_que_get_by_name;

 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
     && fdwrstr(cso, dom_id) != -1 
     && fdwrstr(cso, lower(login)) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp == 'F'
     && fdrdstr(cso, que) != -1 ) {
   return err_no;
 }
 if( resp == 'Z' ) return err_empty;
 return resp ? err_read : err_write;
}
#if 0
/**
 *
 */
uint8_t cdbdaemon::ui_edu_get( const string &uid, edu_info & info ) {
 resp = 0;
 cmd = cmd_ui_edu_get;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
     && fdwrstr(cso, loc) != -1
     && fdwrstr(cso, uid) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp) 
     && resp == 'F' 
     && fdrdstr(cso, info.id) != -1
     && fdrdstr(cso, info.name) != -1 ) {
   return err_no;
 }
 return resp ? err_write : err_read;
}

/**
 *
 */
uint8_t cdbdaemon::ui_work_get( const string &uid, work_info & info ) {
 resp = 0;
 cmd = cmd_ui_work_get;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
     && fdwrstr(cso, loc) != -1
     && fdwrstr(cso, uid) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp) 
     && resp == 'F' 
     && fdrdstr(cso, info.id) != -1
     && fdrdstr(cso, info.name) != -1 ) {
   return err_no;
 }
 return resp ? err_write : err_read;
}

/**
 *
 */
uint8_t cdbdaemon::ui_area_get( const string &uid, area_info & info ) {
 resp = 0;
 cmd = cmd_ui_area_get;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
     && fdwrstr(cso, loc) != -1
     && fdwrstr(cso, uid) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp) 
     && resp == 'F' 
     && fdrdstr(cso, info.id) != -1
     && fdrdstr(cso, info.name) != -1 ) {
   return err_no;
 }
 return resp ? err_write : err_read;
}

/**
 *
 */
uint8_t cdbdaemon::ui_sex_get( const string &uid, sex_info & info ) {
 resp = 0;
 cmd = cmd_ui_sex_get;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
     && fdwrstr(cso, loc) != -1
     && fdwrstr(cso, uid) != -1
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp) 
     && resp == 'F' 
     && fdrdstr(cso, info.id) != -1
     && fdrdstr(cso, info.name) != -1 ) {
   return err_no;
 }
 return resp ? err_write : err_read;
}

/**
 *
 */
uint8_t cdbdaemon::ui_ints_get( const string &uid, vector<int_info> & infos ) {
 int_info info;
 vector<int_info>::size_type cnt;
 
 infos.clear();
 resp = 0;
 cmd = cmd_ui_ints_get;
 if( fdwrite(cso, &cmd, sizeof(cmd)) == sizeof(cmd)
     && fdwrstr(cso, loc) != -1
     && fdwrstr(cso, uid) != -1 
     && fdread(cso, &resp, sizeof(resp)) == sizeof(resp)
     && resp = 'F'
     && fdread(cso, &cnt, sizeof(cnt)) == sizeof(cnt)) {
   cnt--;
   do {
     if( fdrdstr(cso, info.id) == -1 
         || fdrdstr(cso, info.name) == -1 ) return err_read;
   } while(cnt--);
   return err_no;
 }
 if( 'Z' == resp ) return err_empty;
 return resp ? err_write : err_read;
}
#endif
