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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdexcept>

#include "cmaildir.h"
#include "cutil.h"

using namespace std;

/**
@memo creates maildir, if op is set to true, opens it also.
@doc mode for mkdir is set to 0700
@return true on success
*/
bool cmaildir::create( const string & n, bool op )
{
        if( n.empty() ) 
                throw logic_error("cmaildir::create got empty maildir's name");
        if( mkdir( n.c_str(), 0700 )
            || mkdir((n+"/cur").c_str(), 0700)
            || mkdir((n+"/new").c_str(), 0700)
            || mkdir((n+"/tmp").c_str(), 0700) ) {
                err = strerror(errno);
                return false;
        }
        err = "";
        return op ? open(n) : true;
}

/**
opens maildir, if maildir is opened it closes it
@return true on success
*/
bool cmaildir::open( const string & md )
{
        if( isopen && ! close() ) return false; 
        if( ! (dcur = opendir((md+"/cur").c_str()))
            || ! (dnew = opendir((md+"/new").c_str()))
            || ! (dtmp = opendir((md+"/tmp").c_str())) ) {
                err = strerror(errno);
                return false;
        }
        isopen = true;
        name = md;
        err = "";
        return true;
}

/**
removes opened maildir
@exception logic_error when rm is called and there's no opened maildir
@return true on success
*/
bool cmaildir::rm()
{
        if( ! isopen )
                throw logic_error("cmaildir::rm: maildir isn't opened");
        if( ! close() ) return false;
        try {
                cutil::rmdirrec( name );
        } catch( const cutil_error & e ) {
                err = e.what();
                return false;
        }
        return true;
}

/**
closes opened dir
@exception logic_error when no maildir is opened
@return true on success
*/
bool cmaildir::close()
{
        if( ! isopen )
                throw logic_error("cmaildir::close: maildir isn't opened");
        if( closedir(dcur) || closedir(dnew) || closedir(dtmp) ) {
                err = strerror(errno);
                return false;
        }
        isopen = false;
        return true;
}

/**
reads dir, ingores names starting form dot
@exception logic_error when dir name is empty
@return true on success
@param n dir name
@param fs filenames
@param app false means that we should clear fs before adding entries
*/
bool cmaildir::lsdir( const string & n, deque<string> & fs, bool app )
{
        if( n.empty() ) throw logic_error("cmaildir::lsdir: n is empty");
        if( ! app ) fs.clear();
}
