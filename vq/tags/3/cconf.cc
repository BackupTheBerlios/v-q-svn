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
cintconf::cintconf(const std::string &fn, const std::string &def )
		: clnconf(fn, def), min(INT_MIN), max(INT_MAX) {
	ival = str2val(def);
}

/**
 *
 */
cintconf::cintconf(const std::string &fn, const string &def,
		int32_t min, int32_t max )
		: clnconf(fn, def), min(min), max(max) {
	ival = str2val(def);
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
			ival = str2val(str);
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
