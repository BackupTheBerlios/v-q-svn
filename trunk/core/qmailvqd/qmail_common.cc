/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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
#include "qmail_common.hpp"
#include "qmail_files.hpp"

#include <pfstream.hpp>
#include <sys.hpp>
#include <conf.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <cstdio>

using std::string;
using std::ios;
using std::cerr;
using std::endl;
using posix::ipfstream;
using posix::opfstream;
using std::rename;
using namespace sys;

/**
 * Remove line from a file in a secure manner.
 * \param in_fn file name
 * \param rm line to remove
 * \param rmempty if true original file will be removed it created one is empty
 * \return 0 on success, 1 file was not changed, anything else means error
 */
char file_rm(const string &in_fn, const char *rm, bool rmempty) {
	ipfstream in(in_fn.c_str());
	if( ! in ) {
			if( errno != ENOENT ) return 111; 
			return 1;
	}
	string out_fn(in_fn+'.'+uniq());
	opfstream out(out_fn.c_str(), ios::trunc);
	if( ! out ) return 111;
	
	string ln, lncmp(*rm ? rm : "");
	bool found = false;
	
	while(getline(in, ln)) {
			if(ln == lncmp) {
					found = true;
					continue;
			}

			out<<ln<<endl;
			if( ! out ) {
					unlink(out_fn.c_str());
					return 111;
			}
	}
	if( in.bad() ) {
			unlink(out_fn.c_str());
			return 111;
	}
	out.close();
	if( ! found ) {
			unlink(out_fn.c_str());
			return 1;
	}
	if( ! out ) {
			unlink(out_fn.c_str());
			return 111;
	}
	
	struct stat st;
	if( rmempty ) {
			if( stat(out_fn.c_str(), &st) ) {
					unlink(out_fn.c_str());
					return 111;
			}
			if( ! st.st_size ) {
					char ret = 2;
					if( unlink(in_fn.c_str()) ) {
							ret = 111;
					}
					unlink(out_fn.c_str());
					return ret;
			}
	}
			
	if( fstat(in.rdbuf()->fd(), &st)
		|| chown(out_fn.c_str(), st.st_uid, st.st_gid)
		|| chmod(out_fn.c_str(), st.st_mode & 07777) ) {
			unlink(out_fn.c_str());
			return 111;
	}

	return rename(out_fn.c_str(), in_fn.c_str()) ? 111 : 0;
}

