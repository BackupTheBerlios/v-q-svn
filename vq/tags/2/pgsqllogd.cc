/*
Copyright (c) 2002 Pawel Niewiadomski
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
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <vector>

#include <pg_config.h>
#include <libpq++.h>

#include "cdaemonlog.h"
#include "conf_vq.h"
#include "conf_pgsql.h"
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
char msg_type=0;
string msg, msg1, dom, user, ip;

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
    slock = open("daemonlog.lock", O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
    if(slock==-1)
            throw runtime_error((string)"setup: open: daemonlog.lock: "+strerror(errno));
    if(lock_exnb(slock)) {
            if( errno == EWOULDBLOCK )
                    throw runtime_error((string)"setup: daemonlog.lock is already locked");
            else throw runtime_error((string)"setup: daemonlog.lock: lock: "+strerror(errno));
    }
    if(unlink("daemonlog")==-1 && errno != ENOENT )
            throw runtime_error((string)"setup: unlink: daemonlog: "+strerror(errno));
    if( (sso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
            throw runtime_error((string)"setup: socket: "+strerror(errno));
    struct sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    strcpy(sun.sun_path, "daemonlog");
    if(bind(sso, (struct sockaddr*) &sun, SUN_LEN(&sun)))
            throw runtime_error((string)"setup: bind: daemonlog: "+strerror(errno));
    if( !(pg = new PgDatabase(conf_pgsql.c_str())) ) 
            throw runtime_error((string)"setup: no memory"); 
    if( pg->ConnectionBad() )
            throw runtime_error((string)"setup: can't connect to PostgreSQL: "+pg->ErrorMessage());
    if( listen(sso, 5) == -1 )
            throw runtime_error((string)"setup: listen: "+strerror(errno));
}

bool msg_ins( char msg_type, const string &ip, const string &msg ) {
    return pg->Exec(((string)"INSERT INTO log (TYPE,MSG,IP) VALUES('"
            +msg_type+"','"+escape(msg)+"','"
            +escape(ip)+"')").c_str()) == PGRES_COMMAND_OK ;
}
bool msg_ins( char msg_type, const string &ip, 
        const string &msg, const string &dom, 
        const string &user ) {
    return pg->Exec(((string)"INSERT INTO log (TYPE,MSG,DOMAIN,LOGIN,IP)"
            +" VALUES('"+msg_type+"','"+escape(msg)+"','"+escape(dom)+
            "','"+escape(user)+"','"
            +escape(ip)+"')").c_str()) == PGRES_COMMAND_OK ;
}
void msg_read() {
    if( fdrdstr(cso, msg) != -1 
        && fdrdstr(cso, ip) != -1 ) {
            if( msg_ins(msg_type, ip, msg) ) {
                    if(fdwrite(cso,&"K",1) != 1) child_we=true;
            } else {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

bool msg2_ins( char msg_type, const string &ip, 
        const string &msg, const string &msg1 ) {
    return pg->Exec(((string)"INSERT INTO log (TYPE,MSG,MSG1,IP) VALUES('"
            +msg_type+"','"+escape(msg)+"','"
            +escape(msg1)+"','"+escape(ip)+"')").c_str()) == PGRES_COMMAND_OK;
}
bool msg2_ins( char msg_type, const string &ip, 
        const string &msg, const string &msg1,
        const string &dom, const string &user) {
    return pg->Exec(((string)"INSERT INTO log (TYPE,MSG,MSG1,DOMAIN,LOGIN,IP)"
            +" VALUES('"+msg_type+"','"+escape(msg)+"','"
            +escape(msg1)+"','"+escape(dom)+"','"+escape(user)
            +"','"+escape(ip)+"')").c_str()) == PGRES_COMMAND_OK;
}
void msg_read2() {
    if( fdrdstr(cso, msg) != -1
        && fdrdstr(cso, msg) != -1
        && fdrdstr(cso, ip) != -1 ) {
            if( msg2_ins(msg_type, ip, msg, msg1) ) {
                    if(fdwrite(cso,&"K",1) != 1) child_we=true;
            } else {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

bool amsg_ins(char msg_type, const string &ip,
        const string &dom, const string &user,
        const string &msg ) {
    return pg->Exec(("INSERT INTO \""+dom+
            +"_log\" (TYPE,LOGIN,MSG,IP) VALUES('"+msg_type+"','"
            +escape(user)+"','"+escape(msg)+"','"+escape(ip)
            +"')").c_str()) == PGRES_COMMAND_OK;
}    
void amsg_read() {
    if( fdrdstr(cso, dom) != -1
        && fdrdstr(cso, user) != -1
        && fdrdstr(cso, msg) != -1
        && fdrdstr(cso, ip) != -1 ) {
            if( alias2dom(pg, dom) == 2 ) {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
                    return; 
            }
            string tb(dom);
            str2tb(tb);
            if( amsg_ins(msg_type, ip, tb, user, msg) 
                || msg_ins(msg_type, ip, msg, dom, user) ) {
                    if(fdwrite(cso,&"K",1) != 1) child_we=true;
            } else {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

bool amsg_ins( char msg_type, const string &ip,
        const string &dom, const string &user,
        const string &msg, const string &msg1 ) {
    return pg->Exec(("INSERT INTO \""+dom
            +"_log\" (TYPE,LOGIN,MSG,MSG1,IP) VALUES('"
            +msg_type+"','"+escape(user)+"','"+escape(msg)+"','"
            +escape(msg1)+"','"+escape(ip)+"')").c_str()) == PGRES_COMMAND_OK;
}
void amsg_read2() {
    if( fdrdstr(cso, dom) != -1
        && fdrdstr(cso, user) != -1
        && fdrdstr(cso, msg) != -1
        && fdrdstr(cso, msg1) != -1
        && fdrdstr(cso, ip) != -1 ) {
            if( alias2dom(pg, dom) == 2 ) {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
                    return; 
            }
            string tb(dom);
            str2tb(tb);
            if( amsg_ins(msg_type, ip, tb, user, msg, msg1 ) 
                || msg2_ins(msg_type, ip, msg, msg1, dom, user) ) {
                    if(fdwrite(cso,&"K",1) != 1) child_we=true;
            } else {
                    if(fdwrite(cso,&"E",1) != 1) child_we=true;
                    pg_ok = false;
            }
    } else child_re = true;
}

/**
*/
void (*cmd_proc( char cmd )) () {
    msg_type = 0;
    switch(cmd) {
    case cdaemonlog::cmd_d:
            if( !msg_type ) msg_type = 'd'; 
    case cdaemonlog::cmd_w:
            if( !msg_type ) msg_type = 'w'; 
    case cdaemonlog::cmd_n:
            if( !msg_type ) msg_type = 'n'; 
    case cdaemonlog::cmd_e:
            if( !msg_type ) msg_type = 'e'; 
            return &msg_read;
    case cdaemonlog::cmd_dstr:
            if( !msg_type ) msg_type = 'd'; 
    case cdaemonlog::cmd_wstr:
            if( !msg_type ) msg_type = 'w'; 
    case cdaemonlog::cmd_nstr:
            if( !msg_type ) msg_type = 'n'; 
    case cdaemonlog::cmd_estr:
            if( !msg_type ) msg_type = 'e'; 
            return &msg_read2;
    case cdaemonlog::cmd_adstr:
            if( !msg_type ) msg_type = 'D'; 
    case cdaemonlog::cmd_awstr:
            if( !msg_type ) msg_type = 'W'; 
    case cdaemonlog::cmd_anstr:
            if( !msg_type ) msg_type = 'N'; 
    case cdaemonlog::cmd_aestr:
            if( !msg_type ) msg_type = 'E'; 
            return &amsg_read2;
    case cdaemonlog::cmd_ad:
            if( !msg_type ) msg_type = 'D'; 
    case cdaemonlog::cmd_aw:
            if( !msg_type ) msg_type = 'W'; 
    case cdaemonlog::cmd_an:
            if( !msg_type ) msg_type = 'N'; 
    case cdaemonlog::cmd_ae:
            if( !msg_type ) msg_type = 'E'; 
            return &amsg_read;
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
