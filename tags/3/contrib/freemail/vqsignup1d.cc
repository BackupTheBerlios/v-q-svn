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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctime>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <vector>

#define HAVE_NAMESPACE_STD 1
#include <pg_config.h>
#include <libpq++.h>

#include <sig.h>
#include <fd.h>
#include <fdstr.h>
#include <util.h>
#include <lock.h>
#include <sys.h>

#include "cdbdaemon.h"
#include "conf_home.h"
#include "freemail_conf.h"
#include "daemon.h"

using namespace std;
using namespace freemail;

// socket
int slock;
// pgsql
PgDatabase *pg = NULL;
// child
bool child_we, child_re, pg_ok;
char msg_type=0;
string id, localeinfo, login, dom_id, country, city, birthday, area_id, work_id;
string edu_id, sex_id, re_que, re_ans, domain;
vector<string> ints;
vector<string>::size_type ints_cnt;

/**
@exception runtime_error if it can't create socket
*/
void setup() throw(runtime_error)
{
    umask(0);
    if(!sig_pipeign() || !sig_chld_nocldwait() ) 
            throw runtime_error((string)"setup: can't set signals: "+strerror(errno));
    if(chdir((conf_home+"/var/run/vqsignup1d").c_str()))
            throw runtime_error((string)"setup: chdir: "
                    +conf_home+"/var/run: "+strerror(errno));
    slock = open("vqsignup1d.lock", O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
    if(slock==-1)
            throw runtime_error((string)"setup: open: vqsignup1d.lock: "+strerror(errno));
    if(lock_exnb(slock)) {
            if( errno == EWOULDBLOCK )
                    throw runtime_error((string)"setup: vqsignup1d.lock is already locked");
            else throw runtime_error((string)"setup: vqsignup1d.lock: lock: "+strerror(errno));
    }
    if(unlink("vqsignup1d.sock")==-1 && errno != ENOENT )
            throw runtime_error((string)"setup: unlink: vqsignup1d.sock: "+strerror(errno));
    if( (sso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
            throw runtime_error((string)"setup: socket: "+strerror(errno));
    struct sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path, "vqsignup1d.sock");
    if(bind(sso, (struct sockaddr*) &sun, SUN_LEN(&sun)))
            throw runtime_error((string)"setup: bind: vqsignup1d.sock: "+strerror(errno));
    if( chmod(sun.sun_path, 0600) )
            throw runtime_error((string)"setup: chmod: vasignup1d.sock: "+strerror(errno));
    if( !(pg = new PgDatabase(ac_pgsql.val_str().c_str())) ) 
            throw runtime_error((string)"setup: no memory"); 
    if( pg->ConnectionBad() )
            throw runtime_error((string)"setup: can't connect to PostgreSQL: "+pg->ErrorMessage());
    if( listen(sso, 5) == -1 )
            throw runtime_error((string)"setup: listen: "+strerror(errno));
}

/**
 * get education levels
 */
void cmd_edu_ls() {
 if( fdrdstr(cso, localeinfo) != -1 ) {
   if( pg->Exec(((string)"SELECT id,name FROM users_info_edu WHERE locale='"
         +escape(localeinfo)+"' ORDER BY name").c_str()) == PGRES_TUPLES_OK ) {
     
     vector<cdbdaemon::edu_info>::size_type i, cnt = pg->Tuples();
     if( cnt ) {
       if( fdwrite(cso, &"F", 1) == 1 
           && fdwrite(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
         const char *ptr;
         
         for( i=0; i < cnt; ++i ) {
           ptr = pg->GetValue(i, 0);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; }
           ptr = pg->GetValue(i, 1);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; }
         }
         return;
       }
     } else {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
   } else pg_ok = false;
 
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 * get works
 */
void cmd_work_ls() {
 if( fdrdstr(cso, localeinfo) != -1 ) {
   if( pg->Exec(((string)"SELECT id,name FROM users_info_work WHERE locale='"
         +escape(localeinfo)+"' ORDER BY name").c_str()) == PGRES_TUPLES_OK ) {
     
     vector<cdbdaemon::edu_info>::size_type i, cnt = pg->Tuples();
     if( cnt ) {
       if( fdwrite(cso, &"F", 1) == 1 
           && fdwrite(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
         const char *ptr;
         
         for( i=0; i < cnt; ++i ) {
           ptr = pg->GetValue(i, 0);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; }
           ptr = pg->GetValue(i, 1);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; }
         }
         return;
       }
     } else {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
   } else pg_ok = false;
 
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 * get interests
 */
void cmd_int_ls() {
 if( fdrdstr(cso, localeinfo) != -1 ) {
   if( pg->Exec(((string)"SELECT id,name FROM users_info_interests WHERE locale='"
         +escape(localeinfo)+"' ORDER BY name").c_str()) == PGRES_TUPLES_OK ) {
     
     vector<cdbdaemon::edu_info>::size_type i, cnt = pg->Tuples();
     if( cnt ) {
       if( fdwrite(cso, &"F", 1) == 1 
           && fdwrite(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
         const char *ptr;
         
         for( i=0; i < cnt; ++i ) {
           ptr = pg->GetValue(i, 0);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; } 
           ptr = pg->GetValue(i, 1);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; }
         }
         return;
       }
     } else {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
   } else pg_ok = false;
 
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 * get interests
 */
void cmd_area_ls() {
 if( fdrdstr(cso, localeinfo) != -1 ) {
   if( pg->Exec(((string)"SELECT id,name FROM users_info_areas WHERE locale='"
         +escape(localeinfo)+"' ORDER BY name").c_str()) == PGRES_TUPLES_OK ) {
     
     vector<cdbdaemon::edu_info>::size_type i, cnt = pg->Tuples();
     if( cnt ) {
       if( fdwrite(cso, &"F", 1) == 1 
           && fdwrite(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
         const char *ptr;
         
         for( i=0; i < cnt; ++i ) {
           ptr = pg->GetValue(i, 0);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; } 
           ptr = pg->GetValue(i, 1);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; } 
         }
         return;
       }
     } else {
       if( fdwrite(cso, &"child_we = true;Z", 1) != 1 ) child_we = true;
       return;
     }
   } else pg_ok = false;
 
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 * get interests
 */
void cmd_sex_ls() {
 if( fdrdstr(cso, localeinfo) != -1 ) {
   if( pg->Exec(((string)"SELECT id,name FROM users_info_sexes WHERE locale='"
         +escape(localeinfo)+"' ORDER BY name").c_str()) == PGRES_TUPLES_OK ) {
     
     vector<cdbdaemon::sex_info>::size_type i, cnt = pg->Tuples();
     if( cnt ) {
       if( fdwrite(cso, &"F", 1) == 1 
           && fdwrite(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
         const char *ptr;
         
         for( i=0; i < cnt; ++i ) {
           ptr = pg->GetValue(i, 0);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; } 
           ptr = pg->GetValue(i, 1);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; } 
         }
         return;
       }
     } else {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
   } else pg_ok = false;
 
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 * get interests
 */
void cmd_dom_ls() {
 if( fdrdstr(cso, localeinfo) != -1 ) {
   if( pg->Exec(((string)"SELECT id,name FROM domains ORDER BY name").c_str()) == PGRES_TUPLES_OK ) {
     
     vector<cdbdaemon::domain_info>::size_type i, cnt = pg->Tuples();
     if( cnt ) {
       if( fdwrite(cso, &"F", 1) == 1 
           && fdwrite(cso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
         const char *ptr;
         
         for( i=0; i < cnt; ++i ) {
           ptr = pg->GetValue(i, 0);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; } 
           ptr = pg->GetValue(i, 1);
           if(!ptr) ptr = "";
           if( fdwrstr(cso, ptr) == -1 ) { child_we=true; return; } 
         }
         return;
       }
     } else {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
   } else pg_ok = false;
 
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 *
 */
void cmd_ui_rm() {
 if( fdrdstr(cso, id) != -1 ) {
   if( pg->Exec(((string)"DELETE FROM users_info WHERE id='"+escape(id)+"'").c_str()) == PGRES_COMMAND_OK ) {
     if( fdwrite(cso, &"K", 1) == 1 ) return;
   } else pg_ok = false;
 
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 *
 */
void cmd_dom_get() {
 if( fdrdstr(cso, id) != -1 ) {
   if( pg->Exec(("SELECT name FROM domains WHERE id='"
         +escape(id)+"' LIMIT 1").c_str()) == PGRES_TUPLES_OK ) {
     if( ! pg->Tuples() ) {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
     const char *did = pg->GetValue(0,0);
     if(!did) did = "";
     if( fdwrite(cso, &"F", 1) == 1
         && fdwrstr(cso, did) != -1 ) {
       return;
     }
     child_we = true;
     return;
   } else pg_ok = false;
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 *
 */
void cmd_dom_get_by_name() {
 if( fdrdstr(cso, id) != -1 ) {
   if( pg->Exec(("SELECT id FROM domains WHERE name='"
         +escape(id)+"' LIMIT 1").c_str()) == PGRES_TUPLES_OK ) {
     if( ! pg->Tuples() ) {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
     const char *did = pg->GetValue(0,0);
     if(!did) did = "";
     if( fdwrite(cso, &"F", 1) == 1
         && fdwrstr(cso, did) != -1 ) {
       return;
     }
     child_we = true;
     return;
   } else pg_ok = false;
   if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 *
 */
void cmd_ui_add() {
 if( fdrdstr(cso, localeinfo) != -1 
     && fdrdstr(cso, login) != -1
     && fdrdstr(cso, dom_id) != -1
     && fdrdstr(cso, country) != -1
     && fdrdstr(cso, city) != -1
     && fdrdstr(cso, birthday) != -1
     && fdrdstr(cso, area_id) != -1
     && fdrdstr(cso, work_id) != -1
     && fdrdstr(cso, edu_id) != -1
     && fdrdstr(cso, sex_id) != -1
     && fdrdstr(cso, re_que) != -1 
     && fdrdstr(cso, re_ans) != -1 
     && fdread(cso, &ints_cnt, sizeof(ints_cnt)) == sizeof(ints_cnt) ) {
   ints.clear();
   if( ints_cnt -- ) {
     string inter;
     do {
       if( fdrdstr(cso, inter) == -1 ) {
         child_re = true;
         return;
       }
       ints.push_back(inter);
     } while( ints_cnt -- );
   }
   if( pg->Exec("BEGIN") == PGRES_COMMAND_OK 
       && pg->Exec("SELECT NEXTVAL('users_info_id_seq')") == PGRES_TUPLES_OK 
       && pg->Tuples() == 1 ) {
     const char *tmp = pg->GetValue(0,0);
     string uid;
     if( ! tmp ) uid = "";
     else uid = tmp;
     // first query call some procedure returning data, that's why we compare it to PGRES_TUPLES_OK
     if( pg->Exec(("INSERT INTO users_info (id,login,domain_id,country,city,"
           "birthday,re_question,re_answer,locale) VALUES('"+escape(uid)+"','"
           +escape(login)+"','"+escape(dom_id)+"','"+escape(country)+"','"
           +escape(city)+"','"+escape(birthday)+"','"+escape(re_que)+"','"
           +escape(re_ans)+"','"+escape(localeinfo)+"')").c_str()) == PGRES_TUPLES_OK 
         && pg->Exec(("INSERT INTO users_info_map_area (user_id,val_id)"
            " VALUES('"+escape(uid)+"','"+escape(area_id)+"')").c_str()) == PGRES_COMMAND_OK
         && pg->Exec(("INSERT INTO users_info_map_work (user_id,val_id)"
            " VALUES('"+escape(uid)+"','"+escape(work_id)+"')").c_str()) == PGRES_COMMAND_OK
         && pg->Exec(("INSERT INTO users_info_map_edu (user_id,val_id)"
            " VALUES('"+escape(uid)+"','"+escape(edu_id)+"')").c_str()) == PGRES_COMMAND_OK
         && pg->Exec(("INSERT INTO users_info_map_sex (user_id,val_id)"
            " VALUES('"+escape(uid)+"','"+escape(sex_id)+"')").c_str()) == PGRES_COMMAND_OK ) {
       ints_cnt = ints.size();
       if( ints_cnt-- ) {
         do {
           if( pg->Exec(("INSERT INTO users_info_map_ints (user_id,val_id)"
               " VALUES('"+escape(uid)+"','"+escape(ints[ints_cnt])+"')").c_str()) != PGRES_COMMAND_OK ) {
             pg_ok = false;
             goto cmd_ui_add_failure;
           }
         } while( ints_cnt -- );
       }
       if( pg->Exec("COMMIT") == PGRES_COMMAND_OK ) {
         if( fdwrite(cso, &"F", 1) == 1
             && fdwrstr(cso, uid) != -1 ) {
           return;
         }
         child_we = true;
         return;
       } else pg_ok = false;
     } else pg_ok = false;
   } else pg_ok = false;
cmd_ui_add_failure:
   if( fdwrite( cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 *
 */
void cmd_ui_chg() {
 if( fdrdstr(cso, localeinfo) != -1 
     && fdrdstr(cso, id) != -1
     && fdrdstr(cso, country) != -1
     && fdrdstr(cso, city) != -1
     && fdrdstr(cso, birthday) != -1
     && fdrdstr(cso, area_id) != -1
     && fdrdstr(cso, work_id) != -1
     && fdrdstr(cso, edu_id) != -1
     && fdrdstr(cso, sex_id) != -1
     && fdrdstr(cso, re_que) != -1 
     && fdrdstr(cso, re_ans) != -1 
     && fdread(cso, &ints_cnt, sizeof(ints_cnt)) == sizeof(ints_cnt) ) {
   ints.clear();
   if( ints_cnt -- ) {
     string inter;
     do {
       if( fdrdstr(cso, inter) == -1 ) {
         child_re = true;
         return;
       }
       ints.push_back(inter);
     } while( ints_cnt -- );
   }
   if( pg->Exec("BEGIN") == PGRES_COMMAND_OK ) {
     // first query call some procedure returning data, that's why we compare it to PGRES_TUPLES_OK
     if( pg->Exec(("UPDATE users_info set country='"+escape(country)
           +"',city='"+escape(city)+"',"
           "birthday='"+escape(birthday)+"',re_question='"+escape(re_que)
           +"',re_answer='"+escape(re_ans)+"',locale='"+escape(localeinfo)+"'"
           " WHERE id='"+escape(id)+"'").c_str()) == PGRES_COMMAND_OK 
         && pg->CmdTuples() == 1
         && pg->Exec(("UPDATE users_info_map_area SET val_id='"+escape(area_id)
           +"' WHERE user_id='"+escape(id)+"'").c_str()) == PGRES_COMMAND_OK
         && pg->CmdTuples() == 1
         && pg->Exec(("UPDATE users_info_map_work SET val_id='"+escape(work_id)
           +"' WHERE user_id='"+escape(id)+"'").c_str()) == PGRES_COMMAND_OK
         && pg->CmdTuples() == 1
         && pg->Exec(("UPDATE users_info_map_edu SET val_id='"+escape(edu_id)
           +"' WHERE user_id='"+escape(id)+"'").c_str()) == PGRES_COMMAND_OK
         && pg->CmdTuples() == 1
         && pg->Exec(("UPDATE users_info_map_sex SET val_id='"+escape(sex_id)+
           +"' WHERE user_id='"+escape(id)+"'").c_str()) == PGRES_COMMAND_OK
         && pg->CmdTuples() == 1
         && pg->Exec(("DELETE FROM users_info_map_ints WHERE user_id='"
           +escape(id)+"'").c_str()) == PGRES_COMMAND_OK ) {
       ints_cnt = ints.size();
       if( ints_cnt-- ) {
         do {
           if( pg->Exec(("INSERT INTO users_info_map_ints (user_id,val_id)"
               " VALUES('"+escape(id)+"','"
               +escape(ints[ints_cnt])+"')").c_str()) != PGRES_COMMAND_OK ) {
             pg_ok = false;
             goto cmd_ui_chg_failure;
           }
         } while( ints_cnt -- );
       }
       if( pg->Exec("COMMIT") == PGRES_COMMAND_OK ) {
         if( fdwrite(cso, &"K", 1) != 1 ) child_we = true;
         return;
       } else pg_ok = false;
     } else pg_ok = false;
   } else pg_ok = false;
cmd_ui_chg_failure:
   if( fdwrite( cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 *
 */
uint8_t ui_work_get(const string &uid, const string &loc, cdbdaemon::work_info & info ) {
 if( pg->Exec(((string)"SELECT id,name,locale FROM users_info_work "
       "WHERE (code=(SELECT code FROM users_info_work WHERE id="
       "(SELECT val_id FROM users_info_map_work WHERE user_id='"+escape(uid)+"'))"
       "AND locale='"+escape(loc)+"') "
       "OR code=(SELECT code FROM users_info_map_work WHERE id="
       "(SELECT val_id FROM users_info_map_work WHERE user_id='"+escape(uid)+"'))").c_str() )
       == PGRES_TUPLES_OK ) {
   int tups, fidx;
   string locale;
   const char *tmp;
   
   if( ! (tups=pg->Tuples()) ) return cdbdaemon::err_empty;
   if( tups > 2 ) return cdbdaemon::err_read;
  
   fidx = 0;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   info.id = tmp;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   info.name = tmp;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   locale = tmp;
   
   if( tups == 1 || loc == locale ) {
     return cdbdaemon::err_no;
   }
   fidx = 0;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   info.id = tmp;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   info.name = tmp;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   return cdbdaemon::err_no;
 } else pg_ok = false;
 return cdbdaemon::err_read;
}

/**
 *
 */
uint8_t ui_area_get(const string &uid, const string &loc, cdbdaemon::area_info & info ) {
 if( pg->Exec(((string)"SELECT id,name,locale FROM users_info_areas "
       "WHERE (code=(SELECT code FROM users_info_areas WHERE id="
       "(SELECT val_id FROM users_info_map_area WHERE user_id='"+escape(uid)+"'))"
       "AND locale='"+escape(loc)+"') "
       "OR code=(SELECT code FROM users_info_map_area WHERE id="
       "(SELECT val_id FROM users_info_map_area WHERE user_id='"+escape(uid)+"'))").c_str() )
       == PGRES_TUPLES_OK ) {
   int tups, fidx;
   string locale;
   const char *tmp;
   
   if( ! (tups=pg->Tuples()) ) return cdbdaemon::err_empty;
   if( tups > 2 ) return cdbdaemon::err_read;
  
   fidx = 0;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   info.id = tmp;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   info.name = tmp;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   locale = tmp;
   
   if( tups == 1 || loc == locale ) {
     return cdbdaemon::err_no;
   }
   fidx = 0;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   info.id = tmp;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   info.name = tmp;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   return cdbdaemon::err_no;
 } else pg_ok = false;
 return cdbdaemon::err_read;
}

/**
 *
 */
uint8_t ui_sex_get(const string &uid, const string &loc, cdbdaemon::sex_info & info ) {
 if( pg->Exec(((string)"SELECT id,name,locale FROM users_info_sexes "
       "WHERE (code=(SELECT code FROM users_info_sexes WHERE id="
       "(SELECT val_id FROM users_info_map_sex WHERE user_id='"+escape(uid)+"'))"
       "AND locale='"+escape(loc)+"') "
       "OR code=(SELECT code FROM users_info_map_sex WHERE id="
       "(SELECT val_id FROM users_info_map_sex WHERE user_id='"+escape(uid)+"'))").c_str() )
       == PGRES_TUPLES_OK ) {
   int tups, fidx;
   string locale;
   const char *tmp;
   
   if( ! (tups=pg->Tuples()) ) return cdbdaemon::err_empty;
   if( tups > 2 ) return cdbdaemon::err_read;
  
   fidx = 0;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   info.id = tmp;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   info.name = tmp;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   locale = tmp;
   
   if( tups == 1 || loc == locale ) {
     return cdbdaemon::err_no;
   }
   fidx = 0;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   info.id = tmp;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   info.name = tmp;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   return cdbdaemon::err_no;
 } else pg_ok = false;
 return cdbdaemon::err_read;
}

/**
 *
 */
uint8_t ui_edu_get(const string &uid, const string &loc, cdbdaemon::edu_info & info ) {
 if( pg->Exec(((string)"SELECT id,name,locale FROM users_info_edu "
       "WHERE (code=(SELECT code FROM users_info_edu WHERE id="
       "(SELECT val_id FROM users_info_map_edu WHERE user_id='"+escape(uid)+"'))"
       "AND locale='"+escape(loc)+"') "
       "OR code=(SELECT code FROM users_info_map_edu WHERE id="
       "(SELECT val_id FROM users_info_map_edu WHERE user_id='"+escape(uid)+"'))").c_str() )
       == PGRES_TUPLES_OK ) {
   int tups, fidx;
   string locale;
   const char *tmp;
   
   if( ! (tups=pg->Tuples()) ) return cdbdaemon::err_empty;
   if( tups > 2 ) return cdbdaemon::err_read;
  
   fidx = 0;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   info.id = tmp;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   info.name = tmp;
   tmp = pg->GetValue(0, fidx++);
   if(!tmp) tmp = "";
   locale = tmp;
   
   if( tups == 1 || loc == locale ) {
     return cdbdaemon::err_no;
   }
   fidx = 0;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   info.id = tmp;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   info.name = tmp;
   tmp = pg->GetValue(1, fidx++);
   if(!tmp) tmp = "";
   return cdbdaemon::err_no;
 } else pg_ok = false;
 return cdbdaemon::err_read;
}

/**
 *
 */
uint8_t ui_ints_get(const string &uid, vector<cdbdaemon::int_info> & infos ) {
 infos.clear();
 if( pg->Exec(((string) "SELECT ui.id,ui.name FROM users_info_map_ints AS umi,"
       "users_info_interests AS ui WHERE umi.user_id='"+escape(uid)+
       "' AND umi.val_id=ui.id").c_str()) == PGRES_TUPLES_OK ) {
   const char *tmp;
   int tups;
   cdbdaemon::int_info inter;
   if( (tups=pg->Tuples()) == 0 ) return cdbdaemon::err_empty;
   tups--;
   do {
     tmp = pg->GetValue(tups, 0);
     if(!tmp) tmp = "";
     inter.id= tmp;
     tmp = pg->GetValue(tups, 1);
     if(!tmp) tmp = "";
     inter.name = tmp;
     infos.push_back(inter);
   } while( tups -- );
   return cdbdaemon::err_no;
 } else pg_ok = false;
 return cdbdaemon::err_read;
}

/**
 *
 */
void cmd_ui_get_by_name() {
 if( fdrdstr(cso, localeinfo) != -1 
     && fdrdstr(cso, login) != -1
     && fdrdstr(cso, dom_id) != -1 ) {
   if( pg->Exec(("SELECT ui.id,ui.country,ui.city,ui.birthday,"
         "ui.re_question,ui.re_answer,ui.locale,uw.id,uw.name,us.id,us.name,"
         "ue.id,ue.name,ua.id,ua.name"
         " from users_info as ui,users_info_work as uw,users_info_edu as ue,"
         "users_info_areas as ua,users_info_sexes as us,"
         "users_info_map_edu as ume,users_info_map_work as umw,"
         "users_info_map_area as uma,users_info_map_sex as ums where"
         " ui.login='"+escape(login)+"' and ui.domain_id='"+escape(dom_id)
         +"' and ui.id=umw.user_id and umw.val_id=uw.id and "
         "ui.id=ume.user_id and ume.val_id=ue.id and ui.id=ums.user_id"
         " and ums.val_id=us.id and ui.id=uma.user_id and uma.val_id=ua.id").c_str()) 
       == PGRES_TUPLES_OK ) {
     if( pg->Tuples() != 1 ) {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
     const char *tmp;
     int fidx = 0;
     cdbdaemon::ui_full ui;
     vector<cdbdaemon::int_info>::size_type icnt;

     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.country = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.city = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.birthday = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.re_que = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.re_ans = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.locale = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.work.id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.work.name = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.sex.id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.sex.name = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.edu.id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.edu.name = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.area.id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.area.name = tmp;
     
     switch( ui_ints_get(ui.id, ui.ints) ) {
     case cdbdaemon::err_no:
     case cdbdaemon::err_empty:
       break;
     default:
       goto cmd_ui_get_by_name_failure;
     }
     
     icnt = ui.ints.size();

     if( fdwrite(cso, &"F", 1) == 1
         && fdwrstr(cso, ui.id) != -1 
         && fdwrstr(cso, ui.country) != -1 
         && fdwrstr(cso, ui.city) != -1
         && fdwrstr(cso, ui.birthday) != -1 
         && fdwrstr(cso, ui.re_que) != -1 
         && fdwrstr(cso, ui.re_ans) != -1
         && fdwrstr(cso, ui.locale) != -1
         && fdwrstr(cso, ui.area.id) != -1
         && fdwrstr(cso, ui.area.name) != -1
         && fdwrstr(cso, ui.work.id) != -1
         && fdwrstr(cso, ui.work.name) != -1
         && fdwrstr(cso, ui.edu.id) != -1
         && fdwrstr(cso, ui.edu.name) != -1
         && fdwrstr(cso, ui.sex.id) != -1
         && fdwrstr(cso, ui.sex.name) != -1
         && fdwrite(cso, & icnt, sizeof(icnt)) == sizeof(icnt) ) {
       if( icnt -- ) {
         do {
           if( fdwrstr(cso, ui.ints[icnt].id) == -1 
               || fdwrstr(cso, ui.ints[icnt].name) == -1 ) {
             child_we = true;
             return;
           }
         } while( icnt-- );
       }
       return;
     } else { 
       child_we = true;
       return;
     }
   } else pg_ok = false;
cmd_ui_get_by_name_failure:
   if( fdwrite( cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 *
 */
uint8_t ui_ints_ids_get(const string &uid, vector<string> & infos ) {
 infos.clear();
 if( pg->Exec(((string) "SELECT umi.val_id FROM users_info_map_ints AS umi"
       " WHERE umi.user_id='"+escape(uid)+
       "'").c_str()) == PGRES_TUPLES_OK ) {
   const char *tmp;
   int tups;
   string inter;
   if( (tups=pg->Tuples()) == 0 ) return cdbdaemon::err_empty;
   tups--;
   do {
     tmp = pg->GetValue(tups, 0);
     if(!tmp) tmp = "";
     inter = tmp;
     infos.push_back(inter);
   } while( tups -- );
   return cdbdaemon::err_no;
 } else pg_ok = false;
 return cdbdaemon::err_read;
}

/**
 *
 */
void cmd_ui_get_by_name1() {
 if( fdrdstr(cso, localeinfo) != -1 
     && fdrdstr(cso, login) != -1
     && fdrdstr(cso, dom_id) != -1 ) {
   if( pg->Exec(("SELECT ui.id,ui.country,ui.city,ui.birthday,"
         "ui.re_question,ui.re_answer,ui.locale,umw.val_id,ums.val_id,"
         "ume.val_id,uma.val_id"
         " from users_info as ui,"
         "users_info_map_edu as ume,users_info_map_work as umw,"
         "users_info_map_area as uma,users_info_map_sex as ums where"
         " ui.login='"+escape(login)+"' and ui.domain_id='"+escape(dom_id)
         +"' and ui.id=umw.user_id and "
         "ui.id=ume.user_id and ui.id=ums.user_id"
         " and ui.id=uma.user_id").c_str()) 
       == PGRES_TUPLES_OK ) {
     if( pg->Tuples() != 1 ) {
       if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
       return;
     }
     const char *tmp;
     int fidx = 0;
     cdbdaemon::user_info ui;
     vector<cdbdaemon::int_info>::size_type icnt;

     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.country = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.city = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.birthday = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.re_que = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.re_ans = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.locale = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.work_id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.sex_id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.edu_id = tmp;
     tmp = pg->GetValue(0,fidx++); if(!tmp) tmp = "";
     ui.area_id = tmp;
     
     switch( ui_ints_ids_get(ui.id, ui.ints_id) ) {
     case cdbdaemon::err_no:
     case cdbdaemon::err_empty:
       break;
     default:
       goto cmd_ui_get_by_name1_failure;
     }
     
     icnt = ui.ints_id.size();

     if( fdwrite(cso, &"F", 1) == 1
         && fdwrstr(cso, ui.id) != -1 
         && fdwrstr(cso, ui.country) != -1 
         && fdwrstr(cso, ui.city) != -1
         && fdwrstr(cso, ui.birthday) != -1 
         && fdwrstr(cso, ui.re_que) != -1 
         && fdwrstr(cso, ui.re_ans) != -1
         && fdwrstr(cso, ui.locale) != -1
         && fdwrstr(cso, ui.area_id) != -1
         && fdwrstr(cso, ui.work_id) != -1
         && fdwrstr(cso, ui.edu_id) != -1
         && fdwrstr(cso, ui.sex_id) != -1
         && fdwrite(cso, & icnt, sizeof(icnt)) == sizeof(icnt) ) {
       if( icnt -- ) {
         do {
           if( fdwrstr(cso, ui.ints_id[icnt]) == -1 ) {
             child_we = true;
             return;
           }
         } while( icnt-- );
       }
       return;
     } else { 
       child_we = true;
       return;
     }
   } else pg_ok = false;
cmd_ui_get_by_name1_failure:
   if( fdwrite( cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}


/**
 *
 */
void cmd_ui_cmp_by_name() {
 if( fdrdstr(cso, localeinfo) != -1 
     && fdrdstr(cso, login) != -1
     && fdrdstr(cso, dom_id) != -1
     && fdrdstr(cso, country) != -1
     && fdrdstr(cso, city) != -1
     && fdrdstr(cso, birthday) != -1
     && fdrdstr(cso, area_id) != -1
     && fdrdstr(cso, work_id) != -1
     && fdrdstr(cso, edu_id) != -1
     && fdrdstr(cso, sex_id) != -1
     && fdrdstr(cso, re_ans) != -1 ) {
   if( pg->Exec(("SELECT 1 FROM users_info as ui,"
         "users_info_map_area as uma,"
         "users_info_map_work as umw,"
         "users_info_map_edu as ume,"
         "users_info_map_sex as ums"
         " WHERE ui.login='"+escape(login)+"'"
         " AND ui.domain_id='"+escape(dom_id)+"'"
         " AND ui.country_up=UPPER('"+escape(country)+"')"
         " AND ui.city_up=UPPER('"+escape(city)+"')"
         " AND birthday='"+escape(birthday)+"'"
         " AND re_answer=UPPER('"+escape(re_ans)+"')"
         " AND uma.user_id=ui.id AND uma.val_id='"+escape(area_id)+"'"
         " AND umw.user_id=ui.id AND umw.val_id='"+escape(work_id)+"'"
         " AND ume.user_id=ui.id AND ume.val_id='"+escape(edu_id)+"'"
         " AND ums.user_id=ui.id AND ums.val_id='"+escape(sex_id)+"'").c_str()) 
       == PGRES_TUPLES_OK ) {
       char resp; 
       if( pg->Tuples() == 1 ) resp = 'K';
       else resp = 'Z';
       if( fdwrite(cso, &resp, 1) != 1 ) child_we = true;
       return;
   } else pg_ok = false;
   if( fdwrite( cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}
/**
 *
 */
void cmd_user_pass_get() {
 if( fdrdstr(cso, domain) != -1 
     && fdrdstr(cso, login) != -1 ) {
   str2tb(domain);
   if( pg->Exec(("SELECT pass FROM "+domain+" WHERE login='"+escape(login)+"'").c_str())
       == PGRES_TUPLES_OK ) {
       if( ! pg->Tuples() ) {
         if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
         return;
       }
       const char *tmp = pg->GetValue(0,0);
       if(!tmp) tmp = "";
       if( fdwrite(cso, &"F", 1) != 1
           || fdwrstr(cso, tmp) == -1 ) child_we = true;
       return;
   } else pg_ok = false;
   if( fdwrite( cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
 *
 */
void cmd_ui_re_que_get_by_name() {
 if( fdrdstr(cso, dom_id) != -1 
     && fdrdstr(cso, login) != -1 ) {
   str2tb(domain);
   if( pg->Exec(("SELECT re_question FROM users_info WHERE "
         "login='"+escape(login)+"' AND domain_id='"+escape(dom_id)+"'").c_str())
       == PGRES_TUPLES_OK ) {
       if( ! pg->Tuples() ) {
         if( fdwrite(cso, &"Z", 1) != 1 ) child_we = true;
         return;
       }
       const char *tmp = pg->GetValue(0,0);
       if(!tmp) tmp = "";
       if( fdwrite(cso, &"F", 1) != 1
           || fdwrstr(cso, tmp) == -1 ) child_we = true;
       return;
   } else pg_ok = false;
   if( fdwrite( cso, &"E", 1) != 1 ) child_we = true;
 } else child_re = true;
}

/**
*/
void (*cmd_proc( char cmd )) () {
    switch(cmd) {
    case cdbdaemon::cmd_edu_ls:
        return &cmd_edu_ls;
    case cdbdaemon::cmd_work_ls:
        return &cmd_work_ls;
    case cdbdaemon::cmd_int_ls:
        return &cmd_int_ls;
    case cdbdaemon::cmd_area_ls:
        return &cmd_area_ls;
    case cdbdaemon::cmd_sex_ls:
        return &cmd_sex_ls;
    case cdbdaemon::cmd_dom_ls:
        return &cmd_dom_ls;
    case cdbdaemon::cmd_ui_rm:
        return &cmd_ui_rm;
    case cdbdaemon::cmd_ui_add:
        return &cmd_ui_add;
    case cdbdaemon::cmd_dom_get:
        return &cmd_dom_get;
    case cdbdaemon::cmd_dom_get_by_name:
        return &cmd_dom_get_by_name;
    case cdbdaemon::cmd_ui_get_by_name:
        return &cmd_ui_get_by_name;
    case cdbdaemon::cmd_ui_get_by_name1:
        return &cmd_ui_get_by_name1;
    case cdbdaemon::cmd_ui_cmp_by_name:
        return &cmd_ui_cmp_by_name;
    case cdbdaemon::cmd_user_pass_get:
        return &cmd_user_pass_get;
    case cdbdaemon::cmd_ui_re_que_get_by_name:
        return &cmd_ui_re_que_get_by_name;
    case cdbdaemon::cmd_ui_chg:
        return &cmd_ui_chg;
    default:
        return NULL;
    }
}
/**
*/
char child(int fd)
{
    char cmd;
    void (*run)();

    cso = fd;
    pg_ok = true;
    child_we = child_re = false;
    if( ! pg && !(pg = new PgDatabase(ac_pgsql.val_str().c_str())) ) {
            cerr << "child: no memory" << endl;
            return 2;
    }
    if( pg->ConnectionBad() ) {
            delete pg; // why pg doesn't have any connect function?
            if( !(pg = new PgDatabase(ac_pgsql.val_str().c_str())) ) {
                    cerr << "child: no memory" << endl;
                    return 2;
            }
            if( pg->ConnectionBad() ) {
                    cerr << "child: can't connect to PostgreSQL: " 
                         << pg->ErrorMessage() << endl;
                    return 2;
            }
    }
    switch( fdread(cso, &cmd, 1) ) {
    case 0: return 1;
    case -1: 
            child_re = true;
            goto child_end;
    }
    // process cmd
    if( ! (run = cmd_proc(cmd)) ) {
            cerr << "child: unknown command: " << (int) cmd << endl;
            return 2;
    }
    (*run)();
    if( ! pg_ok ) {
            cerr << "child: PostgreSQL error: "<< pg->ErrorMessage() << endl;
            pg_ok = true;
            pg->Exec("ABORT"); // if there's any transaction abort then
    }
    return 0;
child_end:
    if( child_re ) {
            cerr << "child: read error: " << strerror(errno) << endl;
    }
    if( child_we ) {
            cerr << "child: write error: " << strerror(errno) << endl;
    }
    return 2;
}
