/*
Copyright (c) 2003 Pawel Niewiadomski
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <string>
#include <cstdio>

#include "pfstream.h"
#include "vq_conf.h"
#include "lock.h"
#include "uniq.h"

using std::string;
using std::ios;
using std::cerr;
using std::endl;
using posix::ipfstream;
using posix::opfstream;
using std::rename;
using namespace vq;

char rcpt_add(const string &in_fn, const char *rcpt) {
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
						&& ! memcmp(ln.data(), rcpt, rcptl) )
							return 1;

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
	} else if( chmod(out_fn.c_str(), ac_qmail_mode.val_int()) ) 
			return 111;

	return rename(out_fn.c_str(), in_fn.c_str()) ? 111 : 0;
}

int main( int ac , char ** av ) {
		try {
				if( ac != 2 ) {
						cerr<<"usage: "<<*av<<" domain"<<endl
							<<"Program adds a line to qmail/control/rcpthosts file."<<endl
							<<"Program returns 0 on success, 1 if entry for given domain exists,"<<endl
							<<"2 if there's more than 50 recipients (in that case it doesn't add),"<<endl
							<<"anything else on error."<<endl;
						return 111;
				}

				string fn(ac_qmail.val_str()+"/control/rcpthosts");

				opfstream lck((fn+".lock").c_str());
				if( ! lck ) return 111;
		
				if( lock_exnb(lck.rdbuf()->fd()) ) return 111;
	
				return rcpt_add(fn, av[1]);
		} catch(...) {
				return 111;
		}
}
