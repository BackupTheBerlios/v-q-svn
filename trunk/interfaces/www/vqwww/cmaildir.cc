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
