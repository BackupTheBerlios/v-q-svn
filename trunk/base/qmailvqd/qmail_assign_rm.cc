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

#include <pfstream.hpp>
#include <conf.hpp>
#include <sys.hpp>
#include <vqmain.hpp>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
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

int vqmain( int ac, char ** av ) {
		try {
				if( ac != 2 ) {
						cerr<<"usage: "<<*av<<" line_to_remove"<<endl
							<<"Program removes a line from qmail/users/assign."<<endl
							<<"Program returns 0 on success, 1 if file was not changed,"<<endl
							<<"2 if file was removed, anything else on error."<<endl
							<<"Files codes are set up during compilation in qmail_files.h"<<endl;
						return 2;
				}

				conf::clnconf qhome(VQ_HOME+"/etc/ivq/qmail/qmail_home",
					QMAIL_HOME);

				string fn(qhome.val_str()+"/users/assign");

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
						string newu(qhome.val_str()+"/bin/qmail-newu");
						char * const args[] = {
								const_cast<char *>(newu.c_str()),
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
