/*
Copyright (c) 2003,2004 Pawel Niewiadomski
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
#include "pfstream.h"
#include "vq_conf.h"
#include "lock.h"
#include "uniq.h"
#include "main.h"

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
using namespace vq;

char assign_add(const string &in_fn, const char *dom, const char *ln_add) {
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
			string::size_type ln_len, doml = strlen(dom);
			
			while(getline(in, ln)) {
					ln_len = ln.length();
					if( ! ln_len ) continue;
					if( ln==dot ) break;

					if( ln[0] == '+'
						&& ln_len > doml+2
						&& ln[doml+1] == '-'
						&& ! memcmp(ln.data()+1, dom, doml) ) {
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
				|| chown(out_fn.c_str(), st.st_uid, st.st_gid)
				|| chmod(out_fn.c_str(), st.st_mode & 07777) ) {
					unlink(out_fn.c_str());
					return 111;
			}
	} else if( chmod(out_fn.c_str(), ac_qmail_mode.val_int()) ) 
			return 111;

	return rename(out_fn.c_str(), in_fn.c_str()) ? 111 : 0;
}

int cppmain( int ac , char ** av ) {
		try {
				if( ac != 3 ) {
						cerr<<"usage: "<<*av<<" domain line_to_add"<<endl
							<<"Program adds a line to qmail/users/assign file."
							<<"Program returns 0 on success, 1 if entry for given domain exists,"
							<<"anything else on error."<<endl;
						return 111;
				}

				string in_fn(ac_qmail.val_str()+"/users/assign");
				
				opfstream lck((in_fn+".lock").c_str());
				if( ! lck ) return 111;
		
				if( lock_exnb(lck.rdbuf()->fd()) ) return 4;
	
				char ret;
				switch( (ret=assign_add(in_fn, av[1], av[2])) ) {
				case 1: return 1;
				case 0: break;
				default:
						return ret;
				}

				pid_t pid;
				string newu(ac_qmail.val_str()+"/bin/qmail-newu");
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
