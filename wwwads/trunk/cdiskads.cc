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
#include <errno.h>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <time.h>

#include "conf_asisext.h"
#include "cdiskads.h"
#include "util.h"
#include "dirs.h"
#include "fdstr.h"

using namespace std;

/**
open next id file or create it
@param d base directory name
@param wd is a directory for graphic files, it must be accesible via HTTP
*/
cdiskads::cdiskads( const char * d, const char * wd ) throw()
        : dir(d), wdir(wd)
{
    ZERO = 0;
    dir += '/';
    wdir += '/';
    cids.name = dir+"cids";
    next.name = dir+"next";
    nextid.name = dir+"nextid";
}

/**
create file, write (uint32_t) 0 to it
@exception cdiskads_runtime if it can't create file or write or rename
@param cids output stream
@param fn file name
*/
void cdiskads::crt( fstream & cids, const string & fn )
        throw(cdiskads_runtime)
{
    string tmp = fn+".tmp";
    const char *ctmp = tmp.c_str();
    cids.open( ctmp, ios::in | ios::out | ios::trunc, 0666 );
    if( ! cids )
            throw cdiskads_runtime((string)"crt: open: " + fn, true);
            
    cids.write(& ZERO, sizeof(ZERO));
    cids.flush();
    cids.seekp(0,ios::beg);
    if( ! cids ) 
            throw cdiskads_runtime((string)"crt: write to: " + fn, true );
    if( rename(ctmp, fn.c_str()) ) 
            throw cdiskads_runtime((string)"crt: rename: " + ctmp + " to: " + fn, true);
}

/**
initialization
@exception cdiskads_runtime if it can't create or read file
*/
bool cdiskads::init()
        throw(cdiskads_runtime)
{
    if( cids.fs.is_open() ) cids.fs.close();
    if( next.fs.is_open() ) next.fs.close();
    if( nextid.fs.is_open() ) nextid.fs.close();
    cids.fs.open( cids.name.c_str(), ios::in | ios::out | ios::nocreate );
    if( ! cids.fs ) crt(cids.fs,cids.name);
    cids.fs.read(&cids.val,sizeof(cids.val));
    if( ! cids.fs ) throw cdiskads_runtime("read from: " + cids.name, true); 
    
    next.fs.open( next.name.c_str(), ios::in | ios::out | ios::nocreate );
    if( ! next.fs ) crt(next.fs,next.name);
    next.fs.read(&next.val,sizeof(next.val));
    if( ! next.fs ) throw cdiskads_runtime("read from: " + next.name, true);
    
    nextid.fs.open( nextid.name.c_str(), ios::in|ios::out|ios::nocreate);
    if( ! nextid.fs ) crt(nextid.fs,nextid.name);
    nextid.fs.read(&nextid.val,sizeof(nextid.val));
    if( ! nextid.fs ) throw cdiskads_runtime("read from: " + nextid.name, true);
    return true;
}

/**
adds a group
@param name group's name
@retrun true on success
@exception cdiskads_runtime if it can't open cids or cats
*/
bool cdiskads::grps_add( const string & name ) throw(cdiskads_runtime)
{
    string nid, ndir;
    if( ! nextid_get(nid) ) {
            errmsg = (string) "grps_add: " + errmsg;
            return false;
    }
    ndir = dir+nid;
    if( mkdir(ndir.c_str(), 0777) ) {
            errmsg = (string) "grps_add: mkdir: " + ndir + ": " + strerror(errno);
            return false;
    }
    if( mkdir((wdir+nid).c_str(), 0777) ) {
            errmsg = (string) "grps_add: mkdir: " + wdir + nid + ": " + strerror(errno);
            return false;
    }
    fstream fname;
    if( ! dumpu32str( ndir+"/name", name ) ) {
            errmsg = (string) "grps_add: write to: " + ndir + "/name: " + strerror(errno);
            return false;
    }
    fname.open((ndir+"/next").c_str(), ios::in|ios::out|ios::trunc, 0666 );
    fname.write(&ZERO, sizeof(ZERO));
    fname.flush();
    if( ! fname ) {
            errmsg= (string) "grps_add: can't create: " + ndir + "/next: "+strerror(errno);
            return false;
    }
    fname.close();
    string cidst_fn = cids.name+".tmp";
    if( ! cp(cids.fs, cidst_fn, fname) )
            return false;
    fname.write(nid.c_str(), id_len);
    fname.seekp(0,ios::beg);
    cids.val++;
    fname.write(&cids.val, sizeof(cids.val));
    fname.flush();
    if( ! fname ) {
            cids.val--;
            errmsg = (string)"grps_add: write to: " + cidst_fn +": " + strerror(errno);
            return false;
    }
    fname.close();
    cids.fs.close();
    if( rename(cidst_fn.c_str(),cids.name.c_str()) ) {
            errmsg = (string)"grps_add: rename: " + cidst_fn + " to: " + cids.name + ": " + strerror(errno);
            cids.val--;
            return false;
    }
    cids.fs.open(cids.name.c_str(),ios::in|ios::out|ios::nocreate);
    if( ! cids.fs ) throw cdiskads_runtime("grps_add: can't reopen: " + cids.name,true);
    return true;
}

/**
returns group names and their ids
@param names groups' names will be push_back to it 
@param ids groups' ids will be push_back to it
*/
bool cdiskads::grps( deque <string> & names, deque <string> & ids)
        throw()
{
    cids.fs.clear();

    cids.fs.seekg(sizeof(cids.val),ios::beg);
    char buf[id_len+1];
    buf[id_len]='\0';
    string name;
    for( size_type i=0; i < cids.val; i++ ) {
            cids.fs.read(buf,id_len);
            if(!cids.fs) {
                    errmsg = (string)"grps: read from: " + cids.name + ": " +strerror(errno);
                    return false;
            }
            if( !getu32str( dir+buf+"/name", name ) ) {
                    errmsg = (string)"grps: read from: " + dir+buf +"/name: " + strerror(errno);
                    return false;
            }
            names.push_back(name);
            ids.push_back(buf);
    }
    return true;
}

/**
*/
bool cdiskads::grps_rm( const string & id ) throw(cdiskads_runtime)
{
    deque<string> ids;
    ids.push_back(id);
    return grps_rm(ids);
}

/** 
removes selected groups (by id)
@exception cdiskads_runtime if it can't reopen cids
@param ids list of groups' ids
*/
bool cdiskads::grps_rm( const deque <string> & ids )
        throw(cdiskads_runtime)
{
    cids.fs.seekg(sizeof(cids.val),ios::beg);
    fstream cidst((cids.name+".tmp").c_str(),ios::in|ios::out|ios::trunc,0666);
    if(!cidst) {
            errmsg = (string)"grps_rm: can't create: " + cids.name + ".tmp: " + strerror(errno);
            return false;
    }
    
    size_type ncids_cnt = cids.val;
    cidst.write(& ZERO, sizeof(ZERO));
    char buf[id_len+1];
    buf[id_len]='\0';
    deque<string> torm, ftocr;
    for( size_type i=0; i < cids.val; i++ ) {
            cids.fs.read(&buf,id_len);
            if(!cids.fs) {
                    errmsg= (string)"grps_rm: read from: " + cids.name + ": " + strerror(errno);
                    return false;
            }
            if( find(ids.begin(),ids.end(),(string)buf) == ids.end() ) { // not found, write it
                    cidst.write(&buf,id_len);
                    if( ! cidst ) {
                            errmsg = (string) "grps_rm: write to: " + cids.name + ".tmp: " + strerror(errno);
                            return false;
                    }
            } else {
                    ncids_cnt--;
                    torm.push_back(dir+buf);
                    ftocr.push_back(wdir+buf+"/rm");
            }
    }
    cidst.seekp(0,ios::beg);
    cidst.write(&ncids_cnt,sizeof(ncids_cnt));
    cidst.flush();
    if(!cidst) {
            errmsg = (string)"grps_rm: write to: " + cids.name + ".tmp: " + strerror(errno);
            return false;
    }
    cidst.close();
    if(rename((cids.name+".tmp").c_str(),cids.name.c_str())) {
            errmsg = (string)"grps_rm: rename: " + cids.name + "{.tmp,}: " + strerror(errno);
            return false;
    }
    cids.fs.close();
    cids.fs.open(cids.name.c_str(),ios::in|ios::out|ios::nocreate);
    if(!cids.fs) throw cdiskads_runtime((string)"grps_rm: can't reopen: "+ cids.name,true);
    (void) rmdirsrec(torm); // don't care whether dirs have been really removed
    (void) touch(ftocr,true);
    cids.val = ncids_cnt;
    return true;
}

/// returns last error message, or nothing if last operation
/// was successful
const char * cdiskads::err() throw()
{
    return errmsg.c_str();
}

/**
Converts buf to it's hexadecimal representation
@param n id, array of id_len bytes
@return converted string
*/
string cdiskads::id2hex( const char * n )
        throw()
{
        const char lut[] = "0123456789abcdef";
        string ret;
        for( uint8_t i=0, s=id_len/2; i < s; i++ ) {
                ret += lut[ (n[i] & 0xf0) >> 4 ];
                ret += lut[ n[i] & 0xf ];
        }
        return ret;
}

/**
copies in to out, if out isn't open creates it with fn as a file name
@return false on error
@param in input stream
@param fn name of a file to create
@param out output stream, if not open file will be created
*/
bool cdiskads::cp(fstream &in, const string & fn, fstream &out)
        throw()
{
    if( ! out.is_open() ) {
            out.open(fn.c_str(), ios::in|ios::out|ios::trunc, 0666);
            if( ! out ) {
                    errmsg = (string) "cp: open: " + strerror(errno);
                    return false;
            }
    } else {
            out.clear();
            in.clear();
            out.seekp(0,ios::beg);
    }
    in.seekg(0,ios::beg);
    out << in.rdbuf();
    if( ! out ) {
            errmsg = (string) "cp: write to: " + fn + ": " + strerror(errno);
            return false;
    }
    if( in.bad() || in.fail() ) {
            errmsg = (string) "cp: read from in: " + strerror(errno);
            return false;
    }
    return true;
}

/**
create next id for group
@return false on error
@param id next group's id
*/
bool cdiskads::nextid_get( string & id ) throw()
{
    nextid.fs.clear();
    id = id2hex((char*)&nextid.val);
    nextid.val++;
    nextid.fs.seekp(0,ios::beg);
    nextid.fs.write(&nextid.val,sizeof(nextid.val));
    nextid.fs.flush();
    if( ! nextid.fs ) {
            errmsg = (string)"nextid_get: write to: " + dir + "/nextid: " + strerror(errno);
            return false;
    }
    return true;
}
/**
add advertisement
@param grp group's id
@param fd file's data
@param mc max clicks value ( 0 means unlimited )
@param ms max shows value ( 0 means unlimited )
@param ttl time to live (in days) ( 0 means unlimited )
@param misc user's info about ad.
@param url 
@exception cdiskads_runtime if it can't reopen
*/
bool cdiskads::ads_add( const string & grp,
        const string & fd, size_type mc, size_type ms,
        size_type ttl,
        const string & misc, const string & url ) throw(cdiskads_runtime)
{
    string nid, ndir;
    if( ! ads_nextid_get(grp, nid) ) return false;
    ndir = dir + grp + "/" + nid + "/";
    if( mkdir(ndir.c_str(), 0777) ) {
            errmsg = (string) "ads_add: mkdir: " + ndir + ": " + strerror(errno);
            return false;
    }
    string aids_fn=dir+grp+"/aids";
    fstream aids(aids_fn.c_str(),ios::in|ios::out|ios::nocreate);
    if(!aids) crt(aids,aids_fn);
    size_type cnt;
    aids.read(&cnt, sizeof(cnt));
    if(!aids) {
            errmsg = (string) "ads_add: read from: " + aids_fn + ": " + strerror(errno);
            return false;
    }
    fstream aidst;
    string aidst_fn = aids_fn + ".tmp";
    if( ! cp(aids, aidst_fn, aidst ) )
            return false;
    aidst.write(&"",1); // unblocked
    aidst.write(nid.c_str(), id_len);
    aidst.seekp(0,ios::beg);
    cnt++;
    aidst.write(&cnt, sizeof(cnt));
    aidst.flush();
    if( ! aidst ) {
            errmsg = (string)"ads_add: write to: " + aidst_fn + ": " + strerror(errno);
            return false;
    }
    
    if(!dumpu32str(ndir+"/misc", misc)) {
            errmsg = (string)"ads_add: write to: " + ndir+"/misc: " + strerror(errno);
            return false;
    }
    if(!dumpu32str(wdir+grp+'/'+nid+".url", url)) {
            errmsg = (string)"ads_add: write to: " + ndir + "/url: " + strerror(errno);
            return false;
    }

    fstream fi((ndir+"/info").c_str(), ios::out|ios::noreplace, 0666 );
    fi.write( &mc, sizeof(mc) ); mc = 0;
    fi.write( &mc, sizeof(mc) );
    fi.write( &ms, sizeof(ms) ); fi.write( &mc, sizeof(mc) );
    if( ttl ) {
            ttl *= 3600*24; // to seconds
            ttl += time(NULL); // should we check errors?
    }
    fi.write( &ttl, sizeof(ttl) );
    fi.flush();
    if( ! fi ) {
            errmsg = (string) "ads_add: write to: " + ndir + "/info: " + strerror(errno);
            return false;
    }
    fi.close(); 
    fi.open((wdir+grp+'/'+nid+conf_asisext).c_str(), ios::out, 0666 );
    fi.write(fd.data(),fd.length());
    fi.flush();
    if(!fi) {
            errmsg = (string) "ads_add: write to: "+wdir+grp+'/'+nid+conf_asisext+": "+strerror(errno);
            return false;
    }
    
    aidst.close();
    aids.close();
    if( rename(aidst_fn.c_str(),aids_fn.c_str()) ) {
            errmsg = (string) "ads_add: rename: "+ aidst_fn +" to: " + aids_fn + ": " + strerror(errno);
            return false;
    }
    return true;
}

/**
remove ad.
*/
bool cdiskads::ads_rm( const string & grp, const string & ad )
        throw(cdiskads_runtime)
{
    deque<string> ads;
    ads.push_back(ad);
    return ads_rm(grp, ads);
}

/**
remove advertisements
@param grp group's id
@param ids list of ids to delete
*/
bool cdiskads::ads_rm( const string & grp, const deque<string> & ids ) throw(cdiskads_runtime)
{
    string gdir=dir+grp+"/", aids_fn=gdir+"/aids";
    fstream aids(aids_fn.c_str(),ios::in|ios::out|ios::nocreate);
    if(!aids) {
            errmsg = (string)"ads_rm: can't open: " + aids_fn + ": " + strerror(errno);
            return false;
    }
    string aidst_fn=aids_fn+".tmp";
    fstream aidst(aidst_fn.c_str(),ios::in|ios::out|ios::trunc,0666);
    if(!aidst) {
            errmsg = (string)"ads_rm: can't create: " + aidst_fn +": " + strerror(errno);
            return false;
    }
    size_type cnt;
    aids.read(&cnt,sizeof(cnt));

    aidst.write(& ZERO, sizeof(ZERO));
    char buf[id_len+1], blk;
    buf[id_len]='\0';
    deque<string> torm, ftocr;
    for( size_type i=0; i < cnt; i++ ) {
            aids.read(&blk,1);
            aids.read(&buf,id_len);
            if(!aids) {
                    errmsg= (string)"ads_rm: read from: " + aids_fn + ": " + strerror(errno);
                    return false;
            }
            if( find(ids.begin(),ids.end(),(string)buf) == ids.end() ) { // not found, write it
                    aidst.write(&blk,1);
                    aidst.write(&buf,id_len);
                    if( ! aidst ) {
                            errmsg = (string) "ads_rm: write to: " + aidst_fn + ": " + strerror(errno);
                            return false;
                    }
            } else {
                    cnt--;
                    torm.push_back(gdir+buf);
                    ftocr.push_back(wdir+grp+'/'+buf+".rm");
            }
    }
    aidst.seekp(0,ios::beg);
    aidst.write(&cnt,sizeof(cnt));
    aidst.flush();
    if(!aidst) {
            errmsg = (string)"ads_rm: write to: " + aidst_fn + ": " + strerror(errno);
            return false;
    }
    aidst.close();
    if(rename(aidst_fn.c_str(),aids_fn.c_str())) {
            errmsg = (string)"ads_rm: rename: " + aids_fn + "{.tmp,}: " + strerror(errno);
            return false;
    }
    aids.close();
    (void) touch(ftocr,true);
    (void) rmdirsrec(torm); // don't care whether dirs have been really removed
    return true;
}

/**
get next ad's id in a specified group
@return false on error
@param grp group's id
@param id next group's id
@exception cdiskads_runtime if it can't create file grp/nextid
*/
bool cdiskads::ads_nextid_get( const string & grp, string & id ) throw()
{
    ads_nextid.open((dir+grp+"/nextid").c_str(), ios::in|ios::out|ios::nocreate );
    if( ! ads_nextid ) crt(ads_nextid,dir+grp+"/nextid");
    ads_nextid.seekg(0,ios::beg);
    size_type nid;
    ads_nextid.read(&nid, sizeof(nid));
    nid++;
    ads_nextid.seekp(0,ios::beg);
    ads_nextid.write(&nid,sizeof(nid));
    ads_nextid.flush();
    if( ! ads_nextid ) {
            ads_nextid.close();
            errmsg = (string)"ads_nextid_get: write to: " + dir + grp + "/nextid: " + strerror(errno);
            return false;
    }
    nid--;
    id = id2hex((char*)&nid);
    ads_nextid.close();
    return true;
}

/**
changes group's name
@param grp group's id
@param n new name
@return false on error
*/
bool cdiskads::grps_chg( const string & grp, const string & n ) throw()
{
    if( ! dumpu32str(dir+grp+"/name",n) ) {
            errmsg = (string)"grps_chg: write to: " + dir+grp+"/name: "+strerror(errno);
            return false;
    }
    return true;
}

/**
increment ad's info
*/
bool cdiskads::ads_info_inc( const string & grp, const string & aid )
        throw()
{
    bool ret = false;
    fstream finfo((dir+grp+'/'+aid+"/info").c_str(), ios::in|ios::out|ios::nocreate);
    if( ! finfo ) return false;
    struct { size_type max, cur; } ms, mc;
    size_type ttl;
    
    finfo.read(&mc.max,sizeof(mc.max));
    finfo.read(&mc.cur,sizeof(mc.cur));
    finfo.read(&ms.max,sizeof(ms.max));
    finfo.read(&ms.cur,sizeof(ms.cur));
    finfo.read(&ttl, sizeof(ttl));
    if( ! finfo ) return false;

    if( (ms.max && ms.max <= ms.cur)
        || (mc.max && mc.max <= mc.cur)
        || (ttl && ttl <= time(NULL)) ) {
            ret = true;
    }

    ms.cur++;
    finfo.seekp(sizeof(ms.max)+sizeof(mc.max)+sizeof(mc.cur), ios::beg);
    finfo.write(&ms.cur,sizeof(ms.cur));
    return ret;
}
/**
check whether specified ad. exists
@return true if yes
@param url contains path to asis file
@param grp group's id
@param aid ad.'s id
@param faids aids file, put pointer should be set on position of blk flag for give ad
*/
bool cdiskads::ads_get_grp_ad(const string &grp, const string &aid,string &url,
                fstream & faids )
        throw()
{
    url = '/'+grp+'/'+aid+conf_asisext;
    if( access((wdir+url).c_str(), R_OK) ) {
            url = "";
            errmsg = (string) "ads_get: access for reading: " + wdir + url + ": "+ strerror(errno);
            return false;
    }
    if( ads_info_inc(grp,aid) ) {
            char blk = 1;
            faids.write(&blk,1);
    }
    return true;
}

/**
check whether specified ad. exists
@return true if yes
*/
bool cdiskads::ads_get_grp_ad(const string &grp, const string &aid, string &url )
        throw()
{
    url = '/'+grp+'/'+aid+conf_asisext;
    if( access((wdir+url).c_str(), R_OK) ) {
            url = "";
            errmsg = (string) "ads_get: access for reading: " + wdir + url + ": "+ strerror(errno);
            return false;
    }
    (void) ads_info_inc(grp,aid);
    return true;
}

/**
get random ad. from specified group
*/
bool cdiskads::ads_get_grp(const string &grp, string & aid, string &url) 
        throw()
{
    string gdir = dir+grp+'/';
    size_type next, cnt, i;
    char blk, buf[id_len+1];
    buf[id_len]='\0';
    url="";
    fstream fnext((gdir+"next").c_str(), ios::in|ios::out|ios::nocreate);
    fstream faids((gdir+"aids").c_str(), ios::in|ios::out|ios::nocreate);
    if( ! fnext ) {
            errmsg = (string)"ads_get:2: can't open: " + gdir + "next: " + strerror(errno); 
            return false;
    }
    if( ! faids ) {
            errmsg = (string)"ads_get:2: can't open: " + gdir + "aids: " + strerror(errno);
            return false;
    }
    fnext.read(&next,sizeof(next));
    faids.seekg(0,ios::beg);
    faids.read(&cnt,sizeof(cnt));
    faids.seekg(sizeof(cnt)+(1+id_len)*next, ios::beg);
    for( i=next; i < cnt; i++ ) {
            faids.read(&blk,1);
            if( ! blk ) {
                    faids.read(buf,id_len);
                    if( faids.good() ) break;
            } else faids.seekg(id_len,ios::cur);
            if( faids.eof() ) {
                    i = cnt;
                    break;
            }
            if( ! faids ) {
                    errmsg = (string)"ads_get:2: read from: " +gdir+"aids: "+strerror(errno);
                    return false;
            }
            
    }
    if( i == cnt ) {
            faids.seekg(sizeof(cnt),ios::beg);
            for( i=0; i < next; i++ ) {
                    faids.read(&blk,1);
                    if( ! blk ) {
                            faids.read(buf,id_len);
                            if( faids.good() ) break; 
                    } else faids.seekg(id_len,ios::cur);
                    if( ! faids ) {
                            errmsg = (string)"ads_get:2: read from: " +gdir+"aids: "+strerror(errno);
                            return false;
                    }
            }
            if( i == next ) {
                    errmsg = "";
                    return false;
            }
    }
    i++;
    fnext.seekp(0,ios::beg);
    fnext.write(&i,sizeof(i));
    faids.seekp(sizeof(next)+(i-1)*(id_len+1), ios::beg);
    aid = buf;
    return ads_get_grp_ad(grp,aid,url,faids);
}

/**
get random ad.
@gid found group's id
@aid found ad's id
*/
bool cdiskads::ads_get(string &gid, string &aid, string &url)
        throw()
{   
    cids.fs.clear();
    next.fs.clear();

    size_type i;
    char buf[id_len+1];
    buf[id_len] = '\0';
    cids.fs.seekg(sizeof(cids.val)+id_len*next.val, ios::beg);
    url = "";
    for( i=next.val; i < cids.val; i++ ) {
            cids.fs.read(buf,id_len);
            gid = buf;
            if( cids.fs.good() && ads_get_grp(gid,aid,url) ) break; 
            if( cids.fs.eof() ) {
                    i = cids.val;
                    break;
            }
            if( ! cids.fs ) {
                    errmsg = (string)"ads_get:3: read from: " +cids.name+ ": "+strerror(errno);
                    return false;
            }
            
    }
    if( next.val > cids.val ) {
            next.val = cids.val;
            i = cids.val;
    }
    cids.fs.clear();
    if( i == cids.val ) {
            cids.fs.seekg(sizeof(cids.val),ios::beg);
            for( i=0; i < next.val; i++ ) {
                    cids.fs.read(buf,id_len);
                    gid = buf;
                    if( cids.fs.good() && ads_get_grp(gid,aid,url) ) break; 
                    if( ! cids.fs ) {
                            errmsg = (string)"ads_get:3: read from: " +cids.name+"aids: "+strerror(errno);
                            return false;
                    }
            }
            if( i == next.val ) {
                    errmsg = "";
                    return false;
            }
    }
    i++;
    next.fs.seekp(0,ios::beg);
    next.fs.write(&i,sizeof(i));
    next.val = i;
    return true;
}
/**
get host url, modify ad's info if it's possible
@param gid group's id
@param aid ad's id
@param url host's url
@return false on error
*/
bool cdiskads::ads_get_url(const string &grp, const string &aid,string &url )
        throw()
{
    string path = wdir + '/'+grp+'/'+aid+".url";
    if( ! getu32str(path, url) ) {
            errmsg = (string) "ads_get_url: read from: " + path + ": "+ strerror(errno);
            return false;
    }
    fstream finfo((dir+grp+'/'+aid+"/info").c_str(), ios::in|ios::out|ios::nocreate);
    if( finfo ) {
            size_type mc_cur;
            finfo.seekg(sizeof(size_type),ios::beg);
            finfo.read(&mc_cur,sizeof(mc_cur));
            if( finfo ) {
                    mc_cur++;
                    finfo.seekp(sizeof(size_type), ios::beg);
                    finfo.write(&mc_cur,sizeof(size_type));
            }
    } else {
            errmsg = (string)"ads_get_url: read from: " + dir+grp+'/'+aid+"/info: " +strerror(errno);
            return false;
    }
    return true;
}

/**
(un)block ad.
@param gid group's id
@param aid ad.'s id
@param blk if not zero block ad., otherwise unblock it
@return true on success
*/
bool cdiskads::ads_blk( const string & gid, const string & aid, uint8_t blk )
        throw()
{
    char buf[id_len+1];
    buf[id_len]='\0';
    fstream aids((dir+gid+"/aids").c_str(),ios::in|ios::out|ios::nocreate);
    cads::size_type cnt;
    aids.read(&cnt,sizeof(cnt));
    for( cads::size_type i=0; i<cnt && aids; i++ ) {
            aids.seekg(1,ios::cur);
            aids.read(buf,id_len);
            if( aid == buf ) {
                    aids.seekp(sizeof(cnt)+i*(id_len+1),ios::beg);
                    blk = blk ? 1 : 0;
                    aids.write(&blk,1);
                    if(!aids) {
                            errmsg = (string)"ads_blk: write to: "+dir+gid+"/aids: "+strerror(errno);
                            return false;
                    }
                    return true;
            }
    }
    if(!aids) {
            errmsg = (string)"ads_blk: read from: " +dir+gid+"/aids: "+strerror(errno);
            return false;
    }
    return true;
}

/**
get all info about all ads. in specified group
@return true on success
@param gid group's id
*/
bool cdiskads::ads_ls( const string & gid, deque<ad_info> & inf )
        throw()
{
    char buf[id_len+1];
    uint8_t blk;
    ad_info ai;
    inf.clear();
    fstream aids((dir+gid+"/aids").c_str(),ios::in|ios::nocreate);
    if(!aids) {
            errmsg = (string)"ads_ls: can't open: " + dir+gid+"/aids: " + strerror(errno);
            return false;
    }
    size_type cnt=0;
    aids.read(&cnt,sizeof(cnt));
    buf[id_len]='\0';
    for( size_type i=0; i < cnt; i ++ ) {
            aids.read(&blk,1);
            aids.read(buf,id_len);
            if(!aids) {
                    errmsg = (string) "ads_ls: read from: "+dir+gid+"/aids: "+strerror(errno);
                    return false;
            }
            ai.aid = buf;
            ai.blk = blk;
            if(! ads_info(gid, ai) )
                    return false;
            inf.push_back(ai);
    }
    if(!aids) {
            errmsg = (string) "ads_ls: read from: " + dir+gid+"/aids: "+strerror(errno);
            return false;
    }
    return true;
}

/**
get info about specified ad. (mc,cc,ms,cs,ttl,misc,url)
@param gid group's id
@param aid ad.'s id
@param ai info about ad.
@return true on success
*/
bool cdiskads::ads_info( const string & gid, ad_info & ai ) 
        throw()
{
    string ga = gid+'/'+ai.aid;
    ifstream fs((dir+ga+"/info").c_str(),ios::nocreate);
    fs.read(&ai.mc,sizeof(ai.mc));
    fs.read(&ai.cc,sizeof(ai.cc));
    fs.read(&ai.ms,sizeof(ai.ms));
    fs.read(&ai.cs,sizeof(ai.cs));
    fs.read(&ai.ttl,sizeof(ai.ttl));
    if(!fs) {
            errmsg = (string) "ads_info: read from: " + dir+ga+"/info: " + strerror(errno);
            return false;
    }
    fs.close();
    fs.open((dir+ga+"/misc").c_str(),ios::nocreate);
    if( !fs || fdrdstr(fs.rdbuf()->fd(), ai.misc) == -1 ) {
            errmsg = (string) "ads_info: read from: " + dir+ga+"/misc: "+strerror(errno);
            return false;
    }
    fs.close();
    fs.open((wdir+ga+".url").c_str(),ios::nocreate);
    if( ! fs || fdrdstr(fs.rdbuf()->fd(), ai.url) == -1 ) {
            errmsg = (string) "ads_info: read from: " + wdir+ga+".url: "+strerror(errno);
            return false;
    }
    fs.close();
    ai.asisurl = (string) "/" + ga + conf_asisext;
    return true;
}

/**
set info about specified ad. (mc,cc,ms,cs,ttl,misc,url)
@param gid group's id
@param aid ad.'s id
@param ai info about ad.
@return true on success
*/
bool cdiskads::ads_chg( const string & gid, const ad_info & ai ) 
        throw()
{
    string ga = gid+'/'+ai.aid;
    ofstream fs((dir+ga+"/info.tmp").c_str(),ios::trunc,0666);
    fs.write(&ai.mc,sizeof(ai.mc));
    fs.write(&ai.cc,sizeof(ai.cc));
    fs.write(&ai.ms,sizeof(ai.ms));
    fs.write(&ai.cs,sizeof(ai.cs));
    size_type ttl = ai.ttl;
    if( ttl ) {
            ttl *= 3600*24; // to seconds
            ttl += time(NULL); // should we check errors?
    }
    fs.write(&ttl,sizeof(ttl));
    fs.flush();
    if(!fs) {
            errmsg = (string) "ads_chg: write to: " + dir+ga+"/info.tmp: " + strerror(errno);
            return false;
    }
    fs.close();
    fs.open((dir+ga+"/misc.tmp").c_str(),ios::trunc);
    if( !fs || fdwrstr(fs.rdbuf()->fd(), ai.misc) == -1 ) {
            errmsg = (string) "ads_chg: write to: " + dir+ga+"/misc.tmp: "+strerror(errno);
            return false;
    }
    fs.close();
    fs.open((wdir+ga+".url.tmp").c_str(),ios::trunc);
    if( ! fs || fdwrstr(fs.rdbuf()->fd(), ai.url) == -1 ) {
            errmsg = (string) "ads_chg: write to: " + wdir+ga+".url.tmp: "+strerror(errno);
            return false;
    }
    fs.close();
    if( rename((dir+ga+"/info.tmp").c_str(),(dir+ga+"/info").c_str()) ) {
            errmsg = (string) "ads_chr: rename: " + dir+ga+"/info{.tmp,}: "+strerror(errno);
            return false;
    }
    if( rename((dir+ga+"/misc.tmp").c_str(),(dir+ga+"/misc").c_str()) ) {
            errmsg = (string) "ads_chr: rename: " + dir+ga+"/misc{.tmp,}: "+strerror(errno);
            return false;
    }
    if( rename((wdir+ga+".url.tmp").c_str(),(wdir+ga+".url").c_str()) ) {
            errmsg = (string) "ads_chr: rename: " + wdir+ga+".url{.tmp,}: "+strerror(errno);
            return false;
    }
    return true;
}
