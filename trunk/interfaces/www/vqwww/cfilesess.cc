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
#include <string>
#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fstream>
#include <dirent.h>
#include <sstream>
#include <stdexcept>

#include "cfilesess.h"
#include "util.h"
#include "dirs.h"
#include "split.h"
#include "vqwww_conf.h"
#include "auto/lmd5.h"

using namespace std;
using namespace vqwww;

/**
 * \param d base baseectory for sessions, it should exist
 */
cfilesess::cfilesess( const string & d ) : base(d)
{
}

/**
 * Checks whether session is valid
 * \param id session's id
 */
bool cfilesess::valid( const string & id )
{
        struct stat st;
        if( ! stat(sessfn(id).c_str(), & st) ) {
                return S_ISDIR(st.st_mode) ? true : false;
        }
        err = strerror(errno);
        return false;
}
/**
 * Converts string to it's hexadecimal representation
 * \param n string to convert
 * \return converted string
 */
string cfilesess::tohex( const string & n )
{
        return tohex(n.data(), n.length());
}

/**
 * Converts string to it's hexadecimal representation
 * \param ptr string to convert
 * \param len string's length
 * \return converted string
 */
string cfilesess::tohex( const char *ptr, unsigned len )
{
        const char lut[] = "0123456789abcdef";
        string ret;
        for( unsigned i=0; i < len; i++ ) {
                ret += lut[ (ptr[i] & 0xf0) >> 4 ];
                ret += lut[ ptr[i] & 0xf ];
        }
        return ret;
}

/**
 * \param n variable's name
 * \param v value
 * \exception logic_error if session isn't open
 * \return true if success, false if there's no such variable, or error occured
 */
bool cfilesess::setval( const string & n, const string & v )
{
        if( id == "" ) 
				throw logic_error( "cfilesess: session hasn't been opened" );

		string fn(id+'/'+tohex(n));
        ofstream o( fn.c_str(), ios::trunc );
        if( ! o || chmod(fn.c_str(), ac_sess_fmode.val_int())) {
                err = strerror(errno);
                return false;
        }
        o << v;
        o.close();
        if( ! o ) {
                err = strerror(errno);
                return false;
        }
        return true;
}

/**
 * \param n variable's name
 * \param v value
 * \return true on success
 * \exception logic_error if session isn't open
 */
bool cfilesess::getval( const string & n, string & v )
{
    if( id.empty() ) throw logic_error( "session hasn't been opened");
    v = "";
    ifstream i( (id+'/'+tohex(n)).c_str() );
    if( ! i ) {
            err = strerror(errno);
            return false;
    }
    ostringstream str;
    str << i.rdbuf();
    if( i.bad() || i.fail() || str.bad() || str.fail() ) {
            err = strerror(errno);
            return false;
    }
    i.close();
    if( i.bad() || i.fail() ) {
            err = strerror(errno);
            return false;
    }
    v = str.str();
    return true;
}
/**
 * Stores bool value
 */
bool cfilesess::setval( const string & n, bool v ) 
{
        string t = id+'/'+tohex(n);
        ofstream o( t.c_str(), ios::trunc );
        o.close();
        if(! o || chmod(t.c_str(), ac_sess_fmode.val_int()) ) {
                err = strerror(errno);
                return false;
        }
        if(chmod( t.c_str(), S_IRUSR | S_IWUSR | (v ? S_IXUSR : 0) )) {
                err = strerror(errno);
                return false;
        }
        return true;
}

/**
 * Gets boolean value
 */
bool cfilesess::getval( const string &n, bool & v ) 
{
        struct stat st;
        if( ! stat((id+'/'+tohex(n)).c_str(), & st) ) {
                v = st.st_mode & S_IXUSR ? 1 : 0;
                return true;
        }
        err = strerror(errno);
        return false;
}

/**
 * remove variable
 * \param n variable's name
 */
bool cfilesess::rmval( const string & n )
{
        if( unlink( (id+'/'+tohex(n)).c_str() ) ) {
                err = strerror(errno);
                return false;
        }
        return true;
}

/**
 * remove session
 */
bool cfilesess::rm()
{
        try {
                rmdirrec(id);
        } catch( const runtime_error & e ) {
                err = e.what();
                return false;
        }
        return true;
}

/**
 * opens session
 * \param id session's id
 * \return true if session is opened
 */
bool cfilesess::open( const string & id )
{
        struct stat st;
        string tmp = sessfn(id);
        if( stat( tmp.c_str(), &st) == -1 ) {
                err = strerror(errno);
                return false;
        }
        if( ! S_ISDIR(st.st_mode) ) {
                err ="no such session";
                return false;
        }
        this->id =  tmp;
        sid = id;
        return true;
}

/**
 * creates and opens session
 * \param id session's id
 * \return true if session is created
 */
bool cfilesess::create( string & id )
{
        id = crtid();
        string tid = base+'/'+split(id, 2, "/", ac_fs_split.val_int())+id;
        if( ! mkdirhier( tid.c_str(), 0700 ) ) {
                id = crtid();
                tid = base+'/'+split(id, 2, "/", ac_fs_split.val_int())+id;
                if(!mkdirhier(tid.c_str(), 0700)) {
                        err = strerror(errno);
                        return false;
                }
        }
        this->id = tid;
        return true;
}
/**
 * create session's id using MD5 algorithm (md5.h)
 * \return session's id
 */
string cfilesess::crtid()
{
        unsigned char dig[16];
        struct timeval tp;
        gettimeofday( & tp, NULL );
        ostringstream o;
        o << tp.tv_usec << getpid() << tp.tv_sec;
        MD5_CTX con;
        MD5Init( & con );
        MD5Update( & con, (const unsigned char *) o.str().c_str(), o.str().length() );
        MD5Final (dig, &con);
        return tohex((char *) dig, sizeof(dig));
}

/**
 * \return err.c_str()
 */
const char * cfilesess::errmsg()
{
        return err.c_str();
}

/**
 * Stores timeval.
 */
bool cfilesess::setval(const string &name, const struct timeval &t) {
        struct timeval ts[2];
        string fn = id+'/'+tohex(name);
        ts[0] = t;
        ts[1] = t;
        if(utimes(fn.c_str(), (struct timeval *) &ts)) {
                if( errno == ENOENT ) {
                        ofstream o(fn.c_str(), ios::trunc );
                        o.close();
                        if( o.bad() || chmod(fn.c_str(), ac_sess_fmode.val_int())) {
                                err = fn+": "+strerror(errno);
                                return false;
                        }
                        if( ! utimes(fn.c_str(), (struct timeval *) &ts) )
                                return true;
                }
                err = fn+": "+strerror(errno);
                return false;
        }
        return true;
}

/**
 * Gets timeval
 */
bool cfilesess::getval(const string &name, struct timeval &t) {
        struct stat st;
        string fn=id+'/'+tohex(name);
        if( stat(fn.c_str(), &st) ) {
                err = fn+": "+strerror(errno);
                return false;
        }
#ifndef __linux__        
        TIMESPEC_TO_TIMEVAL(&t, &st.st_mtimespec);
#else
        t.tv_sec = st.st_mtime;
        t.tv_usec = 0;
#endif
        return true;
}

/**
 * store binary value
 */
bool cfilesess::setval(const string &n, const char *ptr, unsigned len ) {
        if( id == "" ) throw logic_error( "cfilesess: session hasn't been opened" );
		string fn(id+'/'+tohex(n));
        ofstream o( fn.c_str(), ios::trunc );
        if( ! o || chmod(fn.c_str(), ac_sess_fmode.val_int())) {
                err = strerror(errno);
                return false;
        }
        o.write(reinterpret_cast<char *>(&len), sizeof(len));
        o.write(ptr,len);
        o.close();
        if( ! o ) {
                err = strerror(errno);
                return false;
        }
        return true;
}

/**
 * Gets string of specified size
 */
bool cfilesess::getval( const string &n, char *ptr, unsigned len ) {
    if( id.empty() ) throw logic_error( "session hasn't been opened");
    ifstream i( (id+'/'+tohex(n)).c_str() );
    if( ! i ) {
            err = strerror(errno);
            return false;
    }
    unsigned slen;
    i.read(reinterpret_cast<char *>(&slen), sizeof(slen));
    if( slen != len ) {
            err = "saved length and specified differs";
            return false;
    }
    i.read(ptr, slen);
    if( ! i ) {
            err = strerror(errno);
            return false;
    }
    i.close();
    if( i.bad() || i.fail() ) {
            err = strerror(errno);
            return false;
    }
    return true;
}

/**
 * Gets string.
 * \param ptr will point to string (must deallocate when not used)
 * \param len will be set to length of read string
 * \param n name of variable
 */
bool cfilesess::getval(const string &n, char **ptr, unsigned & len) {
    if( id.empty() ) throw logic_error( "session hasn't been opened");
    ifstream i( (id+'/'+tohex(n)).c_str() );
    if( ! i ) {
            err = strerror(errno);
            return false;
    }
    i.read(reinterpret_cast<char *>(&len), sizeof(len));
    *ptr = new char[len];
    i.read(*ptr, len);
    if( ! i ) {
            err = strerror(errno);
            return false;
    }
    i.close();
    if( i.bad() || i.fail() ) {
            err = strerror(errno);
            return false;
    }
    return true;
}
/**
 * \param id session's id
 * \return path to session directory
 */
string cfilesess::sessfn( const string & id ) {
    return base + '/' + split(id, 2, "/", ac_fs_split.val_int()) + id;
}

/**
 * creates session object
 */
csess* csess::alloc() {
	return new cfilesess(ac_fs.val_str());
}
