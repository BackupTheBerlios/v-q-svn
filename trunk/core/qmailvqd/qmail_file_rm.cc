/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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
#include "qmail_common.hpp"

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
using namespace sys;

int vqmain( int ac , char ** av ) {
		try {
				if( ac != 3 ) {
						cerr<<"usage: "<<*av<<" file's_code line_to_remove"<<endl
							<<"Program removes a line from specified file."<<endl
							<<"Program returns 0 on success, 1 if line was not found, "<<endl
							<<"2 if file was removed, anything else on error."<<endl
							<<"Files codes are set up during compilation in qmail_files.h"<<endl;
						return 111;
				}

				conf::clnconf qhome(VQ_HOME+"/etc/ivq/qmail/qmail_home",
					"/var/qmail/");

				string fn(qhome.val_str()+'/'
					+qf2file(static_cast<qmail_file>(*av[1])));

				opfstream lck((fn+".lock").c_str());
				if( ! lck ) return 111;
		
				if( lock_exnb(lck.rdbuf()->fd()) ) return 111;
	
				return file_rm(fn, av[2]);
		} catch(...) {
				return 111;
		}
}
