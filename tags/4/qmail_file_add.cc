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
#include "qmail_files.h"

using std::string;
using std::ios;
using std::cerr;
using std::endl;
using posix::ipfstream;
using posix::opfstream;
using std::rename;
using namespace vq;

char file_add(const string &in_fn, const char *rm) {
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
	} else if( chmod(out_fn.c_str(), ac_qmail_mode.val_int()) ) {
			unlink(out_fn.c_str());
			return 111;
	}
			
	return rename(out_fn.c_str(), in_fn.c_str()) ? 111 : 0;
}

int main( int ac , char ** av ) {
		try {
				if( ac != 3 ) {
						cerr<<"usage: "<<*av<<" file's_code line_to_add"<<endl
							<<"Program adds a line to specified file."<<endl
							<<"Does not add if file include given line."<<endl
							<<"Program returns 0 on success, 1 if file was not changed,"<<endl
							<<"anything else on error."<<endl
							<<"Files codes are set up during compilation in qmail_files.h"<<endl;
						return 2;
				}

				string fn(ac_qmail.val_str()+'/'
					+qf2file(static_cast<qmail_file>(*av[1])));

				opfstream lck((fn+".lock").c_str());
				if( ! lck ) return 3;
		
				if( lock_exnb(lck.rdbuf()->fd()) ) return 4;
	
				return file_add(fn, av[2]);
		} catch(...) {
				return 111;
		}
}
