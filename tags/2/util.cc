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
#include <dirent.h>
#include <errno.h>
#include <stdexcept>
#include <unistd.h>
#include <algorithm>

#include "util.h"

using namespace std;

/**
write uint32_t to file
@return true on success
*/
bool dumpuint32_t(const string & fn, uint32_t v, int perm, ios::openmode om )
        throw()
{
    ofstream o(fn.c_str(), om, perm);
    o.write(&v,sizeof(v));
    o.flush();
    if(!o) return false;
    return true;
}
/**
write string (including \0) to file
@return true on success
*/
bool dumpstring0(const string & fn, const string & v, int perm,
        ios::openmode om )
        throw()
{
    ofstream o(fn.c_str(), om, perm);
    o.write(v.c_str(),v.length()+1);
    o.flush();
    if(!o) return false;
    return true;
}
/**
write string to file
@return true on success
*/
bool dumpstring(const string & fn, const string & v, int perm,
        ios::openmode om )
        throw()
{
    ofstream o(fn.c_str(), om, perm);
    o.write(v.c_str(),v.length());
    o.flush();
    if(!o) return false;
    return true;
}

/**
write string to file
@return true on success
*/
bool dumpu32str(const string & fn, const string & v, int perm,
        ios::openmode om )
        throw()
{
    ofstream o(fn.c_str(), om, perm);
    string::size_type len = v.length();
    o.write(&len,sizeof(len));
    o.write(v.c_str(),len);
    o.flush();
    if(!o) return false;
    return true;
}

/**
read from file
*/
bool getu32str( const string & fn, string & v ) throw()
{
    v = "";
    char c;
    ifstream i(fn.c_str(),ios::nocreate);
    string::size_type len;
    i.read(&len,sizeof(len));
    v.reserve(len);
    if( len-- ) {
            do {
                    i.get(c);
                    if(!i) return false;
                    v += c;
            } while(len--);
    }
    return true;
}
/**
@param fs files list
@return true on success
*/
bool unlink( const deque<string> & fs ) throw()
{
    deque<string>::size_type fsc = fs.size();
    if( fsc-- ) {
            do {
                    if( ::unlink(fs[fsc].c_str()) )
                            return false;
            } while( fsc-- );
    }
    return true;
}
/**
@param files list to be touched
@param ie ignore errors
@param perm permissions for open
@return true on success
*/
bool touch( const deque<string> & fs, bool ie, int perm ) throw()
{
    fstream t;
    deque<string>::size_type fsc = fs.size();
    if( fsc-- ) {
            do {
                    t.open(fs[fsc].c_str(), ios::out|ios::noreplace, perm );
                    if( ! ie && ! t )
                            return false;
                    t.close();
            } while( fsc-- );
    }
    return true;
}

/**
*/
string escape(const string &w) throw()
{
    string r;
    string::size_type i, wl = w.length();
    r.reserve(wl*2);
    for( i=0; i < wl; i++ ) {
            if( w[i] == '\'' || w[i] == '\"' || w[i] == '\\' ) 
                    r+= '\\';
            r += w[i];
    }
    return r;
}

/**
*/
void str2tb(string &s) throw()
{
    replace(s.begin(),s.end(),'.','_');
    replace(s.begin(),s.end(),'-','_');
}
