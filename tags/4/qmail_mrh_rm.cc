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
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <cerrno>
#include <string>
#include <cstdio>

#include "pfstream.h"
#include "vq_conf.h"
#include "lock.h"
#include "uniq.h"
#include "qmail_common.h"

using std::string;
using std::ios;
using std::cerr;
using std::endl;
using posix::ipfstream;
using posix::opfstream;
using std::rename;
using namespace vq;

int main( int ac , char ** av ) {
		try {
				if( ac != 2 ) {
						cerr<<"usage: "<<*av<<" line_to_remove"<<endl
							<<"Program removes a line from qmail/control/morercpthosts."<<endl
							<<"Program returns 0 on success, 1 if file was not changed,"<<endl
							<<"2 if file was removed, anything else on error."<<endl
							<<"Files codes are set up during compilation in qmail_files.h"<<endl;
						return 2;
				}

				string fn(ac_qmail.val_str()+"/control/morercpthosts");

				opfstream lck((fn+".lock").c_str());
				if( ! lck ) return 3;
		
				if( lock_exnb(lck.rdbuf()->fd()) ) return 4;
	
				char ret=file_rm(fn, av[1]);
				if(ret == 2) {
						if( unlink((fn+".cdb").c_str()) ) {
								return 111;
						}
						return 2;
				} else if( ret == 0 ) {
						string newmrh(ac_qmail.val_str()+"/bin/qmail-newmrh");
						char * const args[] = {
								const_cast<char *>(newmrh.c_str()),
								NULL
						};
						pid_t pid;
						switch((pid=vfork())) {
						case -1:
								return 111;
						case 0:
								execv(*args, args);
								_exit(111);
						}
						while( wait(&pid) == -1 && errno == EINTR );
						if( ! WIFEXITED(pid) || WEXITSTATUS(pid) )
								return 111;
				}
				return ret;
		} catch(...) {
				return 111;
		}
}
