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
#include "qmail_files.hpp"

#include <pfstream.hpp>
#include <conf.hpp>
#include <vqmain.hpp>
#include <sys.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <string>
#include <cstdio>

using std::string;
using std::ios;
using std::cerr;
using std::endl;
using posix::ipfstream;
using posix::opfstream;
using std::rename;
using namespace sys;

char file_add(const string &in_fn, const char *rm, mode_t qmode) {
	ipfstream in(in_fn.c_str());
	bool enoent = false;
	if( ! in ) {
			if( errno != ENOENT ) return 111; 
			enoent = true;
	}
	string out_fn(in_fn+'.'+uniq());
	opfstream out(out_fn.c_str(), ios::trunc);
	if( ! out ) return 111;
	
	string ln, lncmp(*rm ? rm : "");
	if( ! enoent ) {
			while(getline(in, ln)) {
					if(ln == lncmp) {
							unlink(out_fn.c_str());
							return 1;
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
	}
	out<<lncmp<<endl;
	out.close();
	if( ! out ) {
			unlink(out_fn.c_str());
			return 111;
	}
	
	if( ! enoent ) {
			struct stat st;
			if( fstat(in.rdbuf()->fd(), &st)
				|| chown(out_fn.c_str(), st.st_uid, st.st_gid)
				|| chmod(out_fn.c_str(), st.st_mode & 07777) ) {
					unlink(out_fn.c_str());
					return 111;
			}
	} else if( chmod(out_fn.c_str(), qmode) ) {
			unlink(out_fn.c_str());
			return 111;
	}
			
	return rename(out_fn.c_str(), in_fn.c_str()) ? 111 : 0;
}

int vqmain( int ac , char ** av ) {
		try {
				if( ac != 3 ) {
						cerr<<"usage: "<<*av<<" file's_code line_to_add"<<endl
							<<"Program adds a line to specified file."<<endl
							<<"Does not add if file includes given line."<<endl
							<<"Program returns 0 on success, 1 if file was not changed,"<<endl
							<<"anything else on error."<<endl
							<<"Files codes are set up during compilation in qmail_files.h"<<endl;
						return 2;
				}
				
				conf::clnconf qhome(VQ_HOME+"/etc/ivq/qmail/qmail_home",
					QMAIL_HOME);
				conf::cintconf qmode(VQ_HOME+"/etc/ivq/qmail/qmode", "0644");

				string fn(qhome.val_str()+'/'
					+qf2file(static_cast<qmail_file>(*av[1])));

				opfstream lck((fn+".lock").c_str());
				if( ! lck ) return 3;
		
				if( lock_exnb(lck.rdbuf()->fd()) ) return 4;
	
				return file_add(fn, av[2], qmode.val_int());
		} catch(...) {
				return 111;
		}
}
