/**
WWWADS

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

@author Pawel Niewiadomski
@version 1
*/
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/time.h>
#include <string>
#include <errno.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <deque>

#include "sig.h"
#include "conf_adsdir.h"
#include "conf_adswwwdir.h"
#include "conf_socket_mode.h"
#include "cdiskads.h"
#include "adsd.h"
#include "getline.h"
#include "fd.h"
#include "fdstr.h"

cads *ads;
int so, nso;
struct sockaddr_un sun, cun;
socklen_t cunl;
uint8_t cmd;
bool cl_rderr, cl_wrerr, cmd_ok;
struct timeval sock_timeout = { 1, 0 };

/**
*/
void cmd_proc_gls()
{
    deque<string> ids, names;
    if( ads->grps(names,ids) ) {
            if( fdwrite(nso,&"F",1) == 1 ) {
                    deque<string>::size_type cnt = ids.size();
                    if( fdwrite(nso,&cnt,sizeof(cnt)) == 4 ) {
                            if( cnt-- )
                                    do {
                                            if( fdwrstr(nso,names[cnt]) 
                                                != names[cnt].length()
                                               || fdwrstr(nso,ids[cnt])
                                                != ids[cnt].length() )
                                                    cl_wrerr = true;
                                    } while(cnt--);
                    } else cl_wrerr = true;
            } else cl_wrerr = true;
    } else {
            cerr << "can't get groups list: " << ads->err() << '\n'; 
            if(fdwrite(nso,&"E",1)!=1) cl_wrerr = true;
    }
}

/**
get ad.
*/
void cmd_proc_aget()
{
    string gid, aid, url;
    bool ok;
    if( fdrdstr(nso, gid) != -1 
        && fdrdstr(nso, aid) != -1 ) {
            if( ! gid.empty() ) {
                    if( ! aid.empty() ) {
                            ok = ads->ads_get_grp_ad(gid,aid,url);
                    } else {
                            ok = ads->ads_get_grp(gid,aid,url);
                    }
            } else {
                    ok = ads->ads_get(gid,aid,url);
            }
            if( ok ) {
                    if( fdwrite(nso,&"F",1) != 1
                        || fdwrstr(nso,url) == -1
                        || fdwrstr(nso,gid) == -1
                        || fdwrstr(nso,aid) == -1 )
                        cl_wrerr = true;
            } else {
                    if( fdwrite(nso,&"E",1) != 1 ) cl_wrerr = true;
                    cerr<< "can't get ad.: gid=" << gid << ",aid=" << aid
                        << ": " << ads->err() << '\n';
            }
    } else cl_rderr = true;
}

/**
get ad.'s url
*/
void cmd_proc_ageturl()
{
    string gid, aid, url;
    if( fdrdstr(nso, gid) != -1 
        && fdrdstr(nso, aid) != -1 ) {
            if( ads->ads_get_url(gid,aid,url) ) {
                    if( fdwrite(nso,&"F",1) != 1
                        || fdwrstr(nso,url) == -1 )
                        cl_wrerr = true;
            } else {
                    if( fdwrite(nso,&"E",1) != 1 ) cl_wrerr = true;
                    cerr<< "can't get ad.'s url: gid=" << gid << ",aid=" << aid
                        << ": " << ads->err() << '\n';
            }
    } else cl_rderr = true;
}
/**
add ad.
*/
void cmd_proc_aadd()
{
    string grp, fd, fdt, misc, url, fdtmp;
    cads::size_type mc, ms, ttl;
    if( fdrdstr(nso, grp) != -1 
        && fdrdstr(nso, fdt) != -1
        && fdrdstr(nso, fdtmp) != -1 
        && fdread(nso, &mc, sizeof(mc)) == sizeof(mc)
        && fdread(nso, &ms, sizeof(ms)) == sizeof(ms)
        && fdread(nso, &ttl, sizeof(ttl)) == sizeof(ttl)
        && fdrdstr(nso, misc) != -1
        && fdrdstr(nso, url) != -1 ) {
            fd = "Content-Type: " + fdt + "\n\n";
            fd += fdtmp;
            if( ads->ads_add(grp, fd, mc, ms, ttl, misc, url) ) {
                    if( fdwrite(nso, &"K", 1) != 1 ) cl_wrerr = true;
            } else {
                    cerr<< "can't add ad.: grp=" << grp << ": " << ads->err() << '\n';
                    if( fdwrite(nso, &"E", 1) != 1 ) cl_wrerr = true;
            }
    } else cl_rderr = true;
}

/**
*/
void cmd_proc_gadd()
{
    string name;
    if( fdrdstr(nso,name) != -1 ) {
            if( ads->grps_add(name) ) {
                    if( fdwrite(nso,&"K",1) != 1 ) cl_wrerr = true;
            } else {
                    if( fdwrite(nso,&"E",1) != 1 ) cl_wrerr = true;
                    cerr << "can't add group: " << name << ": " << ads->err() << '\n';
            }
    } else cl_rderr = true;
}

/**
*/
void cmd_proc_grm1()
{
    string id;
    if( fdrdstr(nso, id) != -1 ) {
            if( ads->grps_rm(id) ) {
                    if( fdwrite(nso,&"K",1) != 1 ) cl_wrerr = true;
            } else {
                    if( fdwrite(nso,&"E",1) != 1 ) cl_wrerr = true;
                    cerr << "can't remove group: " << id << ": " << ads->err() << endl;
            }
    } else cl_rderr = true;
}

/**
*/
void cmd_proc_arm1()
{
    string gid, aid;
    if( fdrdstr(nso, gid) != -1
        && fdrdstr(nso, aid) != -1 ) {
            if( ads->ads_rm(gid,aid) ) {
                    if( fdwrite(nso,&"K",1) != 1 ) cl_wrerr = true;
            } else {
                    if( fdwrite(nso,&"E",1) != 1 ) cl_wrerr = true;
                    cerr<< "can't remove ad.: grp=" << gid <<";aid=" << aid 
                        << ": " << ads->err() << endl;
            }
    } else cl_rderr = true;
}

/**
*/
void cmd_proc_ablk1()
{
    string gid, aid;
    char blk;
    if( fdrdstr(nso, gid) != -1
        && fdrdstr(nso, aid) != -1
        && fdread(nso, &blk,1) != -1 ) {
            if( ads->ads_blk(gid,aid,blk) ) {
                    if( fdwrite(nso,&"K",1) != 1 ) cl_wrerr = true;
            } else {
                    if( fdwrite(nso,&"E",1) != 1 ) cl_wrerr = true;
                    cerr<< "can't "<< (blk ? "" : "un") << "block ad.: grp="
                        << gid <<";aid=" << aid 
                        << ": " << ads->err() << endl;
            }
    } else cl_rderr = true;
}

/**
*/
void cmd_proc_ainf1()
{
    string gid, aid;
    cads::ad_info ai;
    if( fdrdstr(nso, gid) != -1
        && fdrdstr(nso, ai.aid) != -1 ) {
            if( ads->ads_info(gid,ai) ) {
                    if( fdwrite(nso,&"F",1) != 1
                        || fdwrstr(nso, ai.misc ) == -1 
                        || fdwrstr(nso, ai.url ) == -1 
                        || fdwrite(nso, &ai.mc, sizeof(ai.mc)) == -1
                        || fdwrite(nso, &ai.cc, sizeof(ai.cc)) == -1 
                        || fdwrite(nso, &ai.ms, sizeof(ai.ms)) == -1 
                        || fdwrite(nso, &ai.cs, sizeof(ai.cs)) == -1 
                        || fdwrite(nso, &ai.ttl, sizeof(ai.ttl)) == -1 )
                            cl_wrerr= true;
            } else {
                    if( fdwrite(nso,&"E",1) != 1 ) cl_wrerr = true;
                    cerr<< "can't get info about ad.: grp="
                        << gid <<";aid=" << aid 
                        << ": " << ads->err() << endl;
           }
    } else cl_rderr = true;
}

/**
*/
void cmd_proc_achg1()
{
    cads::ad_info ai;
    string gid;
    if( fdrdstr(nso, gid) != -1
        && fdrdstr(nso, ai.aid) != -1
        && fdrdstr(nso, ai.misc ) != -1
        && fdrdstr(nso, ai.url ) != -1
        && fdread(nso, &ai.mc, sizeof(ai.mc)) != -1
        && fdread(nso, &ai.cc, sizeof(ai.cc)) != -1
        && fdread(nso, &ai.ms, sizeof(ai.ms)) != -1
        && fdread(nso, &ai.cs, sizeof(ai.cs)) != -1
        && fdread(nso, &ai.ttl, sizeof(ai.ttl)) != -1 ) {
            if( ads->ads_chg(gid, ai) ) {
                    if( fdwrite(nso, &"K", 1) == -1 ) cl_wrerr = true;
            } else {
                    if( fdwrite(nso,&"E",1) == -1 ) cl_wrerr = true;
                    cerr << "can't change ad.'s info: grp=" << gid
                        << ";aid=" << ai.aid << ": " << strerror(errno) << endl;
            }
    } else cl_rderr = true;
}

/**
*/
void cmd_proc_als()
{
    string gid;
    if( fdrdstr(nso, gid) != -1 ) {
            deque<cads::ad_info> ais;
            if( ads->ads_ls(gid, ais) ) {
                    deque<cads::ad_info>::size_type cnt = ais.size();
                    if( fdwrite(nso,&"F",1) == 1
                        && fdwrite(nso, &cnt, sizeof(cnt)) == sizeof(cnt) ) {
                            for( deque<cads::ad_info>::size_type i =0; i < cnt; i++ ) {
                                    if( fdwrstr(nso,ais[i].aid) == -1
                                        || fdwrstr(nso, ais[i].misc ) == -1 
                                        || fdwrstr(nso, ais[i].url ) == -1 
                                        || fdwrstr(nso, ais[i].asisurl ) == -1 
                                        || fdwrite(nso, &ais[i].mc, sizeof(ais[i].mc)) == -1
                                        || fdwrite(nso, &ais[i].cc, sizeof(ais[i].cc)) == -1 
                                        || fdwrite(nso, &ais[i].ms, sizeof(ais[i].ms)) == -1 
                                        || fdwrite(nso, &ais[i].cs, sizeof(ais[i].cs)) == -1 
                                        || fdwrite(nso, &ais[i].ttl, sizeof(ais[i].ttl)) == -1
                                        || fdwrite(nso, &ais[i].blk, sizeof(ais[i].blk)) == -1 ) {
                                            cl_wrerr=true;
                                            break;
                                    }
                            }
                    } else cl_wrerr= true;
            } else {
                    if( fdwrite(nso,&"E",1) != 1 ) cl_wrerr = true;
                    cerr << "can't get ad.'s info; grp=" << gid << ": " << ads->err() << endl;
            }
    } else cl_rderr = true;
}

/**
process command
*/
void cmd_proc()
{
    switch(cmd) {
    case ADSD_GADD:
        cmd_proc_gadd();
        break;
    case ADSD_GLS:
        cmd_proc_gls();
        break;
    case ADSD_AADD:
        cmd_proc_aadd();
        break;
    case ADSD_AGET:
        cmd_proc_aget();
        break;
    case ADSD_AGETURL:
        cmd_proc_ageturl();
        break;
    case ADSD_GRM1:
        cmd_proc_grm1();
        break;
    case ADSD_ALS:
        cmd_proc_als();
        break;
    case ADSD_ARM1:
        cmd_proc_arm1();
        break;
    case ADSD_ABLK1:
        cmd_proc_ablk1();
        break;
    case ADSD_AINF1:
        cmd_proc_ainf1();
        break;
    case ADSD_ACHG1:
        cmd_proc_achg1();
        break;
    default:
        fdwrite(nso, &"E", 1);
        cerr << "unknown command: "<< (int) cmd << '\n';
        cmd_ok = false;
    }
}

/**
process child request
@param fd descriptor
*/
void child()
{
    int rr;
    try {
            cmd_ok = true;
            cl_wrerr = cl_rderr = false;
            while( (rr=fdread(nso,&cmd,1)) == 1 && cmd_ok 
                    && ! cl_wrerr && ! cl_rderr ) {
                    cmd_proc();
            }
            if( rr == -1 || cl_rderr ) {
                    cerr << "read from client: " << strerror(errno) << '\n';
            }
            if( cl_wrerr ) {
                    cerr << "write to client: " << strerror(errno) << '\n';
            }
    } catch( exception & e ) {
            cerr << "exception in child: " << e.what() << '\n';
    }
    close(nso);
}

/**
*/
int main( int ac, char **av )
{
    int ret=0;
    try {
            if( chdir(conf_adsdir.c_str()) )
                    throw runtime_error((string)"chdir to: "+conf_adsdir+": "+strerror(errno));
            int fd = open("lock", O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
            if( fd == -1 )
                    throw runtime_error((string)"can't create lock file: "+strerror(errno));
            if( flock(fd, LOCK_EX | LOCK_NB) == -1 ) {
                    if( errno == EWOULDBLOCK )
                            throw runtime_error("another process locked lock");
                    throw runtime_error((string)"can't lock lock: "+strerror(errno));
            }
            ads = new cdiskads(".", conf_adswwwdir.c_str());
            if( ! ads->init() )
                    throw runtime_error((string)"can't initialize ads: "+ads->err());
            if( unlink("socket") == -1 && errno != ENOENT )
                    throw runtime_error((string)"can't unlink socket: "+strerror(errno));
            if( (so = socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
                    throw runtime_error((string)"can't create socket: " + strerror(errno));
            sun.sun_family = AF_UNIX;
            strcpy(sun.sun_path, "socket");
            if(bind(so, (struct sockaddr*) &sun,SUN_LEN(&sun)))
                    throw runtime_error((string)"can't bind to: "+conf_adsdir+"/socket: "+strerror(errno));
            if(chmod("socket", conf_socket_mode_i))
                    throw runtime_error((string)"chmod: socket: "+conf_socket_mode_s+": "+strerror(errno));
            if(listen(so,5))
                    throw runtime_error((string)"can't listen: "+strerror(errno));
            if( ! sig_chld_nocldwait() ) 
                    throw runtime_error((string)"can't set SIGCHLD: "+strerror(errno));
            if( ! sig_pipeign() )
                    throw runtime_error((string)"can't ignore SIGPIPE: "+strerror(errno));
            for(;;) {
                    cunl = sizeof(cun);
                    if( (nso=accept(so,(struct sockaddr*) &cun, &cunl)) != -1 ) {
                            if( setsockopt(nso, SOL_SOCKET, SO_SNDTIMEO, & sock_timeout, sizeof(sock_timeout))
                                || setsockopt(nso, SOL_SOCKET, SO_RCVTIMEO, & sock_timeout, sizeof(sock_timeout)) ) {
                                    cerr << "setsockopt: " << strerror(errno) << endl;
                            }
                            child();
                    } else {
                            if(errno!=EINTR)
                                    cerr<<"accept: "<<strerror(errno)<<'\n';
                    }
            }
    } catch( exception & e ) {
            cerr<< "exception: " << e.what() << '\n';
            ret=1;
    }
    _exit(111);
}

