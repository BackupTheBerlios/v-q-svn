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
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "cdaemonauth.h"
#include "conf_vq.h"
#include "conf_pgsql.h"
#include "conf_qmail_hyp.h"
#include "sig.h"
#include "fd.h"
#include "fdstr.h"
#include "util.h"
#include "lock.h"
#include "pgsqlcommon.h"
#include "sys.h"
#include "daemon.h"

using namespace std;

// socket
int slock;
// pgsql
PgDatabase *pg = NULL;
// child
bool child_we, child_re, pg_ok;
string dom, user, pass, ip, ext;
char type;

/**
@exception runtime_error if it can't create socket
*/
void setup() throw(runtime_error)
{
    umask(0);
    if(!sig_pipeign() || !sig_chld_nocldwait() ) 
            throw runtime_error((string)"setup: can't set signals: "+strerror(errno));
    if(chdir((conf_vq+"/sockets").c_str()))
            throw runtime_error((string)"setup: chdir: "
                    +conf_vq+"/sockets: "+strerror(errno));
    slock = open("daemonauth.lock", O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
    if(slock==-1)
            throw runtime_error((string)"setup: open: daemonauth.lock: "+strerror(errno));
    if(lock_exnb(slock)) {
            if( errno == EWOULDBLOCK )
                    throw runtime_error((string)"setup: daemonauth.lock is already locked");
            else throw runtime_error((string)"setup: daemonauth.lock: lock: "+strerror(errno));
    }
    if(unlink("daemonauth")==-1 && errno != ENOENT )
            throw runtime_error((string)"setup: unlink: daemonauth: "+strerror(errno));
    if( (sso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
            throw runtime_error((string)"setup: socket: "+strerror(errno));

    struct sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path, "daemonauth");
    if(bind(sso, (struct sockaddr*) &sun, SUN_LEN(&sun)))
            throw runtime_error((string)"setup: bind: daemonauth: "+strerror(errno));
    if( !(pg = new PgDatabase(conf_pgsql.c_str())) ) 
            throw runtime_error((string)"setup: no memory"); 
    if( pg->ConnectionBad() )
            throw runtime_error((string)"setup: can't connect to PostgreSQL: "+pg->ErrorMessage());
    if( listen(sso, 5) == -1 )
            throw runtime_error((string)"setup: listen: "+strerror(errno));
}

/**
*/
void cmd_dom_add() throw()
{
    if( fdrdstr(cso, dom) != -1 ) {
            str2tb(dom);
            if( pg->Exec(((string)"SELECT DOM_ADD('"+escape(dom)+"')").c_str()) 
                == PGRES_TUPLES_OK ) {
                    if( fdwrite(cso, &"K",1) != 1 ) child_we=true;
            } else {
                    if( fdwrite(cso, &"E",1) != 1 ) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

/**
*/
void cmd_dom_rm() throw()
{
    if( fdrdstr(cso, dom) != -1 ) {
            str2tb(dom);
            if( pg->Exec(((string)"SELECT DOM_RM('"+escape(dom)+"')").c_str()) 
                == PGRES_TUPLES_OK ) {
                    if( fdwrite(cso, &"K",1) != 1 ) child_we=true;
            } else {
                    if( fdwrite(cso, &"E",1) != 1 ) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

/**
*/
void cmd_user_add() throw()
{
    uint8_t flags;
    ostringstream sflags;
    if( fdrdstr(cso, user) != -1
        && fdrdstr(cso, dom) != -1
        && fdrdstr(cso, pass) != -1 
        && fdread(cso, &flags, sizeof(flags)) == sizeof(flags) ) {
            str2tb(dom);
            sflags<<(int)flags;
            if( pg->Exec(((string)"SELECT USER_ADD('"+escape(user)+"','"
                        +escape(dom)+"','"+escape(pass)+"','"
                        +sflags.str()+"')").c_str()) 
                == PGRES_TUPLES_OK && pg->Tuples() == 1 ) {
                    const char * ret = pg->GetValue(0,0);
                    if( ret && *ret == '1' && ! *(ret+1) ) {
                            if( fdwrite(cso, &"D", 1) != 1 ) child_we = true;
                    } else if( fdwrite(cso, &"K",1) != 1 ) child_we=true;
            } else {
                    if( fdwrite(cso, &"E",1) != 1 ) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

/**
*/
void cmd_user_pass() throw()
{
    if( fdrdstr(cso, user) != -1
        && fdrdstr(cso, dom) != -1
        && fdrdstr(cso, pass) != -1 ) {
            str2tb(dom);
            if( pg->Exec(((string)"SELECT USER_PASS('"+escape(user)+"','"
                        +escape(dom)+"','"+escape(pass)+"')").c_str()) 
                == PGRES_TUPLES_OK ) {
                    if( fdwrite(cso, &"K",1) != 1 ) child_we=true;
            } else {
                    if( fdwrite(cso, &"E",1) != 1 ) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

/**
*/
void cmd_user_rm() throw()
{
    if( fdrdstr(cso, user) != -1
        && fdrdstr(cso, dom) != -1 ) {
            str2tb(dom);
            if( pg->Exec(((string)"SELECT USER_RM('"+escape(user)+"','"
                        +escape(dom)+"')").c_str()) 
                == PGRES_TUPLES_OK ) {
                    if( fdwrite(cso, &"K",1) != 1 ) child_we=true;
            } else {
                    if( fdwrite(cso, &"E",1) != 1 ) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

/**
*/
void cmd_user_auth()
{
    string tb;
    if( fdrdstr(cso, user) != -1
       && fdrdstr(cso, dom) != -1 ) {
            if( alias2dom(pg, dom) == 2 ) {
                    if(fdwrite(cso,&"E",1)!=1) child_we=true;
                    pg_ok = false;
                    return;
            }
            tb=dom;
            str2tb(tb);
            if( pg->Exec(("SELECT login,pass,flags FROM "+tb
                        +" WHERE login='"+escape(user)+"'").c_str()) != PGRES_TUPLES_OK ) {
                    if(fdwrite(cso,&"E",1)!=1) child_we=true;
                    pg_ok = false;
                    return;
            }
            if( pg->Tuples() == 1 ) {
                    const char *pass = pg->GetValue(0,1);
                    if(!pass) pass = "";
                    const char *flags = pg->GetValue(0,2);
                    if(!flags) flags= "0";
                    int iflags = atoi(flags);
                    if( fdwrite(cso,&"F",1) != 1
                        || fdwrstr(cso, user) == -1
                        || fdwrstr(cso, dom) == -1
                        || fdwrstr(cso, pass) == -1
                        || fdwrite(cso, &iflags, sizeof(iflags)) == -1 ) 
                            child_we = true;
            } else if(fdwrite(cso,&"Z",1)!=1) child_we=true;
    } else child_re = true;
}

/**
 */
void cmd_user_ex() {
    if( -1 != fdrdstr(cso, dom)
        && -1 != fdrdstr(cso, user) ) {
            uint8_t ret=0xff;
            if( alias2dom(pg, dom) != 2 ) {
                    str2tb(dom);
                    if (pg->Exec(("SELECT user_exist('"+escape(dom)
                            +"','"+escape(user)+"')").c_str()) == PGRES_TUPLES_OK
                        && pg->Tuples() == 1) {
                            istringstream is;
                            unsigned ret1;
                            const char *tmp = pg->GetValue(0,0);
                            if(!tmp) tmp = "";
                            is.str(tmp);
                            is>>ret1;
                            if(is) ret = ret1 & 0xff;
                    }
                    if( 1 != fdwrite(cso, &"F", 1) 
                       || sizeof(ret) != fdwrite(cso, &ret, sizeof(ret)) )
                            child_we = true;
            } else if( 1 != fdwrite(cso, &"E", 1) ) child_we = true;
    } else child_re = true;
}
/**
*/
void cmd_dom_ip_add()
{
    if( fdrdstr(cso,dom) != -1
        && fdrdstr(cso,ip) != -1 ) {
            if( pg->Exec(("INSERT INTO ip2domain (DOMAIN,IP) VALUES('"+escape(dom)
                          +"','"+escape(ip)+"')").c_str()) == PGRES_COMMAND_OK ) {
                    if(fdwrite(cso,&"K",1) != 1) child_we=true;
            } else {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

/**
*/
void cmd_dom_ip_rm()
{
    if( fdrdstr(cso,dom) != -1
        && fdrdstr(cso,ip) != -1 ) {
            if( pg->Exec(("DELETE FROM ip2domain WHERE domain='"+escape(dom)
                          +"' AND ip='"+escape(ip)+"'").c_str()) == PGRES_COMMAND_OK ) {
                    if(fdwrite(cso,&"K",1) != 1) child_we=true;
            } else {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
            }
    } else child_re=true;
}
/**
*/
void cmd_dom_ip_rm_all()
{
    if( fdrdstr(cso,dom) != -1 ) {
            if( pg->Exec(("DELETE FROM ip2domain WHERE domain='"+escape(dom)
                          +"'").c_str()) == PGRES_COMMAND_OK ) {
                    if(fdwrite(cso,&"K",1) != 1) child_we=true;
            } else {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
            }
    } else child_re=true;
}
/**
*/
void cmd_dom_ip_ls()
{
    if( fdrdstr(cso,dom) == -1 ) {
            child_re=true;
            return;
    }
    if( pg->Exec(("SELECT ip FROM ip2domain WHERE domain='"+escape(dom)
                            +"'ORDER BY ip").c_str()) == PGRES_TUPLES_OK ) {
            vector<string>::size_type i, cnt = pg->Tuples();
            if( fdwrite(cso,&"F",1) == -1
                || fdwrite(cso,&cnt,sizeof(cnt)) == -1 ) {
                    child_we = true;
                    return;
            }
            string::size_type ipl;
            const char *ip;
            for(i=0; i<cnt;i++) {
                    ip = pg->GetValue(i,0);
                    ipl = ! ip ? 0 : strlen(ip);
                    if( fdwrite(cso,&ipl,sizeof(ipl)) == -1 ) {
                            child_we=true;
                            return;
                    }
                    if(ip) {
                            if( fdwrite(cso, ip, ipl) == -1 ) {
                                    child_we=true;
                                    return;
                            }
                    }
            }
    } else {
            if(fdwrite(cso,&"E",1)!=1) child_we=true;
            pg_ok=false;
    }
}
/**
*/
void cmd_dom_ip_ls_dom()
{
    if( pg->Exec("SELECT DISTINCT domain FROM ip2domain ORDER BY domain") == PGRES_TUPLES_OK ) {
            vector<string>::size_type i, cnt = pg->Tuples();
            if(fdwrite(cso,&"F",1) == -1
               || fdwrite(cso,&cnt,sizeof(cnt)) == -1 ) {
                    child_we = true;
                    return;
            }
            string::size_type doml;
            const char *dom;
            for(i=0; i<cnt;i++) {
                    dom = pg->GetValue(i,0);
                    doml= !dom ? 0 : strlen(dom);
                    if( fdwrite(cso,&doml,sizeof(doml)) == -1 ) {
                            child_we=true;
                            return;
                    }
                    if(dom) {
                            if( fdwrite(cso, dom, doml) == -1 ) {
                                    child_we=true;
                                    return;
                            }
                    }
            }
    } else {
            if(fdwrite(cso,&"E",1)!=1) child_we=true;
            pg_ok=false;
    }
}

/**
 *
 */
void cmd_udot_add() throw (out_of_range) {
    string val;
    if( fdrdstr(cso, dom) == -1
       || fdrdstr(cso, user) == -1
       || fdrdstr(cso, ext) == -1
       || fdread(cso, &type, sizeof(type)) == -1 
       || fdrdstr(cso, val) == -1 ) {
            child_re = true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("SELECT udot_add('"
                    +escape(dom)+"','"+escape(user)+"','"
                    +escape(ext)+"','\\"+type+"','"
                    +escape(val)+"')").c_str()) == PGRES_TUPLES_OK 
        && pg->Tuples() == 1 ) {
            const char *id = pg->GetValue(0,0);
            if(! id) id = "";
            if(fdwrite(cso,&"F",1) == -1
               || fdwrstr(cso, id) == -1 ) {
                    child_we=true;
            }
            return;
    }
    if(fdwrite(cso,&"E",1)!=1) child_we=true;
    pg_ok=false;
}
/**
 *
 */
void cmd_udot_ls() throw(out_of_range) {
    string ext;
    if( fdrdstr(cso, dom) == -1
       || fdrdstr(cso, user) == -1
       || fdrdstr(cso, ext) == -1 ) {
            child_re = true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("SELECT ID,TYPE,VALUE FROM "+dom+"_dot " 
                    " WHERE ext='"+escape(user)+'\\'+conf_qmail_hyp.at(0)
                    +escape(ext)+"' ORDER BY id").c_str()) ) {
            vector<string>::size_type i, cnt = pg->Tuples();
            if(fdwrite(cso,&"F",1) == -1
               || fdwrite(cso,&cnt,sizeof(cnt)) == -1 ) {
                    child_we = true;
                    return;
            }
            const char *tmp;
            cvq::udot_info ui;
            for(i=0; i<cnt;i++) {
                    tmp = pg->GetValue(i,0);
                    ui.id = !tmp ? "" : tmp;
                    tmp = pg->GetValue(i,1);
                    ui.type = !tmp ? 0 : *tmp;
                    tmp = pg->GetValue(i,2);
                    ui.val = !tmp ? "" : tmp;
                    if( fdwrstr(cso,ui.id) == -1 
                        || fdwrite(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
                        || fdwrstr(cso, ui.val) == -1 ) {
                            child_we=true;
                            return;
                    }
            }
            return;
    }
    if(fdwrite(cso,&"E",1)!=1) child_we=true;
    pg_ok=false;
}
/**
 *
 */
void cmd_udot_ls_type() throw(out_of_range) {
    if( fdrdstr(cso, dom) == -1
       || fdrdstr(cso, user) == -1
       || fdrdstr(cso, ext) == -1
       || fdread(cso, &type, sizeof(type)) == -1 ) {
            child_re = true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("SELECT ID,TYPE,VALUE FROM "+dom+"_dot " 
                    " WHERE ext='"+escape(user)+'\\'+conf_qmail_hyp.at(0)
                    +escape(ext)+"' AND type='\\"+type+"' ORDER BY id").c_str()) ) {
            vector<string>::size_type i, cnt = pg->Tuples();
            if(fdwrite(cso,&"F",1) == -1
               || fdwrite(cso,&cnt,sizeof(cnt)) == -1 ) {
                    child_we = true;
                    return;
            }
            const char *tmp;
            cvq::udot_info ui;
            for(i=0; i<cnt;i++) {
                    tmp = pg->GetValue(i,0);
                    ui.id = !tmp ? "" : tmp;
                    tmp = pg->GetValue(i,1);
                    ui.type = !tmp ? 0 : *tmp;
                    tmp = pg->GetValue(i,2);
                    ui.val = !tmp ? "" : tmp;
                    if( fdwrstr(cso,ui.id) == -1 
                        || fdwrite(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
                        || fdwrstr(cso, ui.val) == -1 ) {
                            child_we=true;
                            return;
                    }
            }
            return;
    }
    if(fdwrite(cso,&"E",1)!=1) child_we=true;
    pg_ok=false;
}
/**
 *
 */
void cmd_udot_rm() {
    string id;
    if( fdrdstr(cso, dom) == -1
       || fdrdstr(cso, id) == -1 ) {
            child_re=true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("DELETE FROM "+dom+"_dot WHERE id='"+escape(id)+"'").c_str()) 
            == PGRES_COMMAND_OK ) {
            if(fdwrite(cso, &"K", 1)!=1) child_we=true;
            return;
    } 
    if(fdwrite(cso, &"E",1)!=1) child_we=true;
    pg_ok =false;
}
/**
 *
 */
void cmd_udot_rm_type() {
    if( fdrdstr(cso, dom) == -1 
        || fdrdstr(cso, user) == -1 
        || fdrdstr(cso, ext) == -1
        || fdread(cso, &type, sizeof(type)) != sizeof(type) ) {
            child_re = true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("DELETE FROM "+dom+"_dot WHERE "
            "ext='"+escape(user)+'\\'+conf_qmail_hyp[0]
            +escape(ext)+"' AND type='\\"+type+"'" ).c_str()) == PGRES_COMMAND_OK ) {
            if(fdwrite(cso, &"K",1) != 1) child_we = true;
            return;
    }
    if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
    pg_ok = false;
}
/**
 *
 */
void cmd_udot_get() {
    string id;
    if( fdrdstr(cso, dom) == -1
       || fdrdstr(cso, id) == -1 ) {
            child_re=true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("SELECT TYPE,VALUE FROM "+dom+"_dot WHERE id='"+escape(id)+"'").c_str())
        == PGRES_TUPLES_OK ) {
            if(!pg->Tuples()) {
                    if(fdwrite(cso, &"Z", 1)!=1) child_we=true;
                    return;
            }
            string val;
            const char *ptr;
            ptr = pg->GetValue(0, 0);
            type = ! ptr ? '\0' : *ptr;
            ptr = pg->GetValue(0, 1);
            val = ! ptr ? "" : ptr;
            if( fdwrite(cso, &"F", 1) != 1 
               || fdwrite(cso, &type, sizeof(type)) != sizeof(type)
               || fdwrstr(cso, val) == -1 ) {
                    child_we=true;
            }
            return;
    }
    if(fdwrite(cso, &"E",1)!=1) child_we=true;
    pg_ok =false;
}
void cmd_udot_rep() {
    cvq::udot_info ui;

    if( fdrdstr(cso, dom) == -1
       || fdrdstr(cso, ui.id) == -1
       || fdread(cso, &ui.type, sizeof(ui.type)) != sizeof(ui.type)
       || fdrdstr(cso, ui.val) == -1 ) {
            child_re=true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("UPDATE "+dom+"_dot SET TYPE='\\"+ui.type
            +"',VALUE='"+escape(ui.val)+"' WHERE id='"
            +escape(ui.id)+"'").c_str()) == PGRES_COMMAND_OK) {
            if( ! pg->CmdTuples() ) {
                    if(fdwrite(cso, &"Z", 1) != 1) child_we=true;
                    return;
            }
            if(fdwrite(cso, &"K", 1) != 1) child_we=true;
            return;
    }
    if(fdwrite(cso, &"E",1)!=1) child_we=true;
    pg_ok =false;
}
/**
 *
 */
void cmd_udot_has() {
    if( fdrdstr(cso, dom) == -1
       || fdrdstr(cso, user) == -1
       || fdrdstr(cso, ext) == -1
       || fdread(cso, &type, sizeof(type)) == -1 ) {
            child_re = true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("SELECT 1 FROM "+dom+"_dot " 
                    " WHERE ext='"+escape(user)+'\\'+conf_qmail_hyp.at(0)
                    +escape(ext)+"' AND type='\\"+type+"' LIMIT 1").c_str()) ) {
            if( pg->Tuples() ) {
                    if(fdwrite(cso, &"T", 1) != 1)
                            child_we = true;
            } else if(fdwrite(cso, &"F",1) != 1)
                    child_we=true;
            return;
    }
    if(fdwrite(cso, &"E",1)!=1) child_we=true;
    pg_ok =false;
}
/**
 *
 */
void cmd_udot_type_cnt() {
    if( fdrdstr(cso, dom) == -1 
        || fdrdstr(cso, user) == -1 
        || fdrdstr(cso, ext) == -1
        || fdread(cso, &type, sizeof(type)) != sizeof(type) ) {
            child_re = true;
            return;
    }
    str2tb(dom);
    if( pg->Exec(("SELECT COUNT(*) FROM "+dom+"_dot WHERE "
            "ext='"+escape(user)+'\\'+conf_qmail_hyp[0]
            +escape(ext)+"' AND type='\\"+type+"'" ).c_str()) 
        == PGRES_TUPLES_OK && pg->Tuples() == 1 ) {
            const char *ptr = pg->GetValue(0,0);
            if( ptr && *ptr ) {
                    cdaemonauth::size_type cnt;
                    istringstream is(ptr);
                    is>>cnt;
                    if(is) {
                            if(fdwrite(cso, &"F",1) != 1
                               || fdwrite(cso, &cnt, sizeof(cnt)) != sizeof(cnt))
                                    child_we = true;
                            return;
                    }
            }
    }
    if( fdwrite(cso, &"E", 1) != 1 ) child_we = true;
    pg_ok = false;
}
/**
*/
void (*cmd_proc( char cmd )) () {
    switch(cmd) {
    case cdaemonauth::cmd_dom_add: 
        return &cmd_dom_add;
    case cdaemonauth::cmd_dom_rm:
        return &cmd_dom_rm;
    case cdaemonauth::cmd_user_add:
        return &cmd_user_add;
    case cdaemonauth::cmd_user_rm:
        return &cmd_user_rm;
    case cdaemonauth::cmd_user_pass:
        return &cmd_user_pass;
    case cdaemonauth::cmd_user_auth:
        return &cmd_user_auth;
    case cdaemonauth::cmd_dom_ip_add:
        return &cmd_dom_ip_add;
    case cdaemonauth::cmd_dom_ip_rm:
        return &cmd_dom_ip_rm;
    case cdaemonauth::cmd_dom_ip_rm_all:
        return &cmd_dom_ip_rm_all;
    case cdaemonauth::cmd_dom_ip_ls:
        return &cmd_dom_ip_ls;
    case cdaemonauth::cmd_dom_ip_ls_dom:
        return &cmd_dom_ip_ls_dom;
    case cdaemonauth::cmd_udot_add:
        return &cmd_udot_add;
    case cdaemonauth::cmd_udot_rm:
        return &cmd_udot_rm;
    case cdaemonauth::cmd_udot_rm_type:
        return &cmd_udot_rm_type;
    case cdaemonauth::cmd_udot_ls:
        return &cmd_udot_ls;
    case cdaemonauth::cmd_udot_ls_type:
        return &cmd_udot_ls_type;
    case cdaemonauth::cmd_udot_get:
        return &cmd_udot_get;
    case cdaemonauth::cmd_udot_rep:
        return &cmd_udot_rep;
    case cdaemonauth::cmd_udot_has:
        return &cmd_udot_has;
    case cdaemonauth::cmd_udot_type_cnt:
        return &cmd_udot_type_cnt;
    case cdaemonauth::cmd_user_ex:
        return &cmd_user_ex;
    default:
        return NULL;
    }
}
/**
 * \return 0 - ok, 1 - eof, 2 - error
 */
char child(int fd)
{
    char cmd;
    void (*run)();

    cso = fd;
    pg_ok = true;
    child_we = child_re = false;
    if( ! pg && !(pg = new PgDatabase(conf_pgsql.c_str())) ) {
            cerr << "child: no memory" << endl;
            return 2;
    }
    if( pg->ConnectionBad() ) {
            delete pg; // why pg doesn't have any connect function?
            if( !(pg = new PgDatabase(conf_pgsql.c_str())) ) {
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

