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

#include <sys/stat.h>
#include <string>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

#ifdef DIRSTEST
#include <iostream>
#endif

#include "dirs.h"

using namespace std;

/**
create directories hierarchy
@return true on success
*/
bool mkdirhier(const char *path, mode_t m)
{
    if( mkdir(path,m) ) {
            if( errno != ENOENT ) return false;
            string pre, sub, p(path);
            string::size_type pos, beg;
            for( beg=0; (pos=p.find('/',beg)) != string::npos; beg = pos+1 ) {
                    sub = p.substr(beg, pos-beg);
                    if( sub == ".." ) return false;
                    if( sub == "." ) continue;
                    pre += sub + '/';
                    if( sub == "" ) continue;
                    if( mkdir( pre.c_str(), m ) && errno != EEXIST ) {
                            return false;
                    }
            }
            pre += p.substr(beg);
            if( mkdir(pre.c_str(), m) ) return false;
            return true;
    }
    return true;
}

/**
create directories hierarchy, change owners for new directories
@return true on success
*/
bool mkdirhier(const char *path, mode_t m, int uid, int gid)
{
    if( mkdir(path,m) ) {
            if( errno != ENOENT ) return false;
            string pre, sub, p(path);
            string::size_type pos, beg;
            for( beg=0; (pos=p.find('/',beg)) != string::npos; beg = pos+1 ) {
                    sub = p.substr(beg, pos-beg);
                    if( sub == ".." ) return false;
                    if( sub == "." ) continue;
                    pre += sub + '/';
                    if( sub == "" ) continue;
                    if( mkdir( pre.c_str(), m ) ) { 
                            if( errno != EEXIST ) return false;
                    } else if( chown(pre.c_str(), uid, gid) ) return false;
            }
            pre += p.substr(beg);
            if( mkdir(pre.c_str(), m)
                || chown(pre.c_str(), uid, gid)) return false;
            return true;
    }
    if( chown(path, uid, gid) ) return false;
    return true;
}

/**
@exception runtime_error error occured, application possibly can't continue
@return false on error which isn't critical
*/
bool rmdirrec( const string & n ) throw(runtime_error)
{
        if( n.empty() ) return true;
        DIR *cd = opendir(".");
        if( ! cd ) return false;

        if( chdir(n.c_str()) ) {
                if( fchdir(dirfd(cd)) ) 
                        throw runtime_error((string)"rmdirrec: can't go back to: " 
                                + n + ": " + strerror(errno));
                closedir(cd);
                return false;
        }
        DIR *d = opendir(".");
        if( ! d ) {
                if( fchdir(dirfd(cd)) )
                        throw runtime_error((string)"rmdirrec: can't go back to: "
                                + n + ": " + strerror(errno) );
                closedir(cd);
                return false;
        }
        struct dirent *de;
        struct stat st;
        while( (de=readdir(d)) ) {
                if( de->d_name[0] == '.' && ( de->d_name[1] == '\0' 
                    || (de->d_name[1] == '.' && de->d_name[2] == '\0' )) )
                        continue;
                        
                if( ::unlink(de->d_name) ) {
                        if( stat(de->d_name, & st) ) {
                                closedir(d);
                                if( fchdir(dirfd(cd)) )
                                        throw runtime_error((string)"rmdirrec: can't go back to: "
                                                + n + ": " + strerror(errno) );
                                closedir(cd);
                                return false;
                        }
                        if( S_ISDIR(st.st_mode) ) {
                                if( ! rmdirrec(de->d_name) ) {
                                        closedir(d);
                                        if( fchdir(dirfd(cd)) )
                                                throw runtime_error((string)"rmdirrec: can't go back to: "
                                                        + n + ": " + strerror(errno) );
                                        closedir(cd);
                                        return false;
                                }
                        } else {
                                closedir(d);
                                if( fchdir(dirfd(cd)) )
                                        throw runtime_error((string)"rmdirrec: can't go back to: "
                                                + n + ": " + strerror(errno) );
                                closedir(cd);
                                return false;
                        }
                }
        }
        closedir(d);
        if( fchdir(dirfd(cd)) ) 
                throw runtime_error((string)"rmdirrec: can't go back to: "
                        + n + ": " + strerror(errno) );
        closedir(cd);
        return rmdir(n.c_str()) ? false : true;
}

#ifdef DIRSTEST
#include <string>
#include <errno.h>
int main()
{
    char *d[] = { "tets/xcg/fdgv/./cvb//bcv", "./sdf/srt" };
    cout << d[0] << ' ' << (int)mkdirhier(d[0],0700) << endl;
    cout << d[0] << ' ' << (int)rmdirrec(d[0]) << ':'<< strerror(errno) <<endl;
    cout << d[0] << ' ' << (int)rmdirrec("tets") << ':'<< strerror(errno) <<endl;
    cout << d[1] << ' ' << (int)mkdirhier(d[1],0700) << endl;
    cout << d[1] << ' ' << (int)rmdirrec(d[1]) << ':' << strerror(errno) << endl;
    cout << d[1] << ' ' << (int)rmdirrec("sdf") << ':' << strerror(errno) << endl;
    return 0;
}
#endif
