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
#include <pfstream.hpp>
#include <vqmain.hpp>
#include <conf.hpp>
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

char rcpt_add(const string &in_fn, const char *rcpt, mode_t qmode) {
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
			string ln;
			string::size_type rcptl = strlen(rcpt);
			uint32_t rcpts=0;
			while(getline(in, ln) && rcpts < 50 ) {
					if(ln.empty()) continue;
					if(ln.length() == rcptl 
						&& ! memcmp(ln.data(), rcpt, rcptl) ) {
							unlink(out_fn.c_str());
							return 1;
					}

					++rcpts;
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
			if( rcpts == 50 ) {
					unlink(out_fn.c_str());
					return 2;
			}
	}
	out<<rcpt<<endl;
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
				if( ac != 2 ) {
						cerr<<"usage: "<<*av<<" domain"<<endl
							<<"Program adds a line to qmail/control/rcpthosts file."<<endl
							<<"Program returns 0 on success, 1 if entry for given domain exists,"<<endl
							<<"2 if there's more than 50 recipients (in that case it doesn't add),"<<endl
							<<"anything else on error."<<endl;
						return 111;
				}

				conf::clnconf qhome(VQ_HOME+"/etc/ivq/qmail/qmail_home",
					"/var/qmail/");
				conf::cintconf qmode(VQ_HOME+"/etc/ivq/qmail/qmode", "0644");

				string fn(qhome.val_str()+"/control/rcpthosts");

				opfstream lck((fn+".lock").c_str());
				if( ! lck ) return 111;
		
				if( lock_exnb(lck.rdbuf()->fd()) ) return 111;
	
				return rcpt_add(fn, av[1], qmode.val_int());
		} catch(...) {
				return 111;
		}
}
