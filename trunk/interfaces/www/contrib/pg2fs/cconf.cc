/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

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

#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <fstream>
#include <climits>
#include <sstream>

#include "cconf.h"

using namespace std;

/**
 *
 */
clnconf::clnconf(const std::string &fn, const std::string &def )
		: cconf(fn), val(def) {
}

/**
 * read first line from a file
 */
std::string clnconf::ln_read() {
	errno = 0;
	ifstream in(fn.c_str());
	if( ! in ) {
			if( errno != ENOENT ) throw read_error(this, fn);
			return val;
	}
	std::string ln;
	getline(in, ln);
	in.close();
	if( in.bad() ) throw read_error(this, fn);
	return ln;
}

/**
 *
 */
std::string clnconf::val_str() {
	if( ! read ) {
			val=ln_read();
			read = true;
	}
	return val;
}

/**
 *
 */
cintconf::cintconf(const std::string &fn, const std::string &def, 
		int32_t mult )
		: clnconf(fn, def), min(INT_MIN), max(INT_MAX), mult(mult) {
	ival = str2val(def)*mult;
}

/**
 *
 */
cintconf::cintconf(const std::string &fn, const string &def,
		int32_t min, int32_t max, int32_t mult )
		: clnconf(fn, def), min(min), max(max), mult(mult) {
	ival = str2val(def)*mult;
}

/**
 *
 */
int32_t cintconf::str2val( const string & str ) {
		if(str.empty()) throw empty_error(this);

		int64_t val;
		istringstream is;
		is.str(str);
		if( str[0] == '0' ) is>>oct>>val;
		else is>>val;
		if( ! is || val < min || val > max ) {
				ostringstream smin, smax;
				smin<<min;
				smax<<max;
				throw range_error(this, str, smin.str(), smax.str());
		}
		return val;
}

/**
 *
 */
string cintconf::val_str() {
	if(! read) val_int();
	return val;
}

/**
 *
 */
int32_t cintconf::val_int() {
	if(! read) {
			string str(ln_read());
			ival = str2val(str)*mult;
			val = str;
			read = true;
	}
	return ival;
}

/**
 *
 */
cuidconf::cuidconf(const std::string &fn, const std::string &def )
		: cintconf(fn, def) {
}

/**
 *
 */
int32_t cuidconf::val_int() {
	if(!read) {
			struct stat st;
			if( ! stat(fn.c_str(), &st) ) {
					if( (int32_t)st.st_uid != ival ) {
							ostringstream os;
							os<<st.st_uid;
							val = os.str();
							ival = st.st_uid;
					}
			} else if(errno != ENOENT)
					throw err(this, "stat: ", strerror(errno));
			read = true;
	}
	return ival;
}

/**
 *
 */
cgidconf::cgidconf(const std::string &fn, const std::string &def ) 
		: cintconf(fn, def) {
}

/**
 *
 */
int32_t cgidconf::val_int() {
	if(!read) {
			struct stat st;
			if( ! stat(fn.c_str(), &st) ) {
					if( (int32_t)st.st_gid != ival ) {
							ostringstream os;
							os<<st.st_gid;
							val = os.str();
							ival = st.st_gid;
					}
			} else if(errno != ENOENT)
					throw err(this, "stat: ", strerror(errno));
			read = true;
	}
	return ival;
}

/**
 *
 */
cmapconf::cmapconf( const string & ln ) : cconf(ln) {
}

/**
 *
 */
const cmapconf::map_type & cmapconf::val_map() {
	if( ! read ) {
			read = true;
			errno = 0;
			ifstream in(fn.c_str());
			if( ! in ) {
					if( errno != ENOENT ) throw read_error(this, fn);
					return map;
			}
			
			string ln;
			string::size_type sep;	
			while( getline(in, ln) ) {
					if(ln.empty()) continue;
					if(ln[0]=='#') continue;
					
					sep = ln.find(':');
					if( sep == string::npos )
							throw err(this, "invalid entry", ln);
					map[ln.substr(0, sep)] = ln.substr(sep+1);
			}
			in.close();
			if( in.bad() ) throw read_error(this, fn);
	}
	return map;
}
