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
#include <pfstream.hpp>
#include <vqmain.hpp>
#include <sys.hpp>
#include <conf.hpp>

#include <sys/stat.h>
#include <sys/wait.h>
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

char assign_add(const string &in_fn, const char *ln_add, mode_t qmode) {
	bool enoent = false;
	ipfstream in(in_fn.c_str());
	if( ! in ) {
			if( errno != ENOENT ) return 111; 
			enoent = true;
	}
	string out_fn(in_fn+'.'+uniq());
	opfstream out(out_fn.c_str(), ios::trunc);
	if( ! out ) return 111;
	if( ! enoent ) {
			string ln, dot(".");
			string::size_type ln_len, ln_add_len = strlen(ln_add);
			
			while(getline(in, ln)) {
					ln_len = ln.length();
					if( ! ln_len ) continue;
					if( ln==dot ) break;

					if( ln_len == ln_add_len
						&& ! memcmp(ln.data(), ln_add, ln_add_len) ) {
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
	out<<ln_add<<endl<<'.'<<endl;
	out.close();
	if( ! out ) {
			unlink(out_fn.c_str());
			return 111;
	}
	
	if( ! enoent ) {
			struct stat st;
			if( fstat(in.rdbuf()->fd(), &st)
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
				if( ac != 2 ) {
						cerr<<"usage: "<<*av<<" line_to_add"<<endl
							<<"Program adds a line to qmail/users/assign file."
							<<"Program returns 0 on success, 1 if entry exists,"
							<<"anything else on error."<<endl;
						return 111;
				}

				conf::clnconf qhome(VQ_ETC_DIR+"/ivq/qmail/qmail_home",
					QMAIL_HOME);
				conf::cintconf qmode(VQ_ETC_DIR+"/ivq/qmail/qmode", "0644");

				string in_fn(qhome.val_str()+"/users/assign");
				
				opfstream lck((in_fn+".lock").c_str());
				if( ! lck ) return 111;
		
				if( lock_exnb(lck.rdbuf()->fd()) ) return 4;
	
				char ret=assign_add(in_fn, av[1], qmode.val_int());
				switch (ret) {
				case 1: return 1;
				case 0: break;
				default:
						return ret;
				}

				pid_t pid;
				string newu(qhome.val_str()+"/bin/qmail-newu");
				char * const args[] = {
						const_cast<char *>(newu.c_str()),
						NULL
				};
						
				switch((pid=vfork())) {
				case -1: 
						return 111;
				case 0:
						execv(*args, args);
						_exit(111);
				}
				while( wait(&pid) == -1 && errno == EINTR );
				if( ! WIFEXITED(pid) || WEXITSTATUS(pid) ) return 111;
				return 0;
		} catch(...) { 
				return 111;
		}
}
