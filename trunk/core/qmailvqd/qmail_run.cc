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
#include "qmail_progs.hpp"

#include <vqmain.hpp>
#include <conf.hpp>

#include <unistd.h>

#include <iostream>
#include <string>

using std::string;
using std::ios;
using std::cerr;
using std::endl;

int vqmain( int ac , char ** av ) {
		try {
				if( ac != 2 ) {
						cerr<<"usage: "<<*av<<" program's_code"<<endl
							<<"Program runs specified program usually under SUID/SGID."<<endl;
						return 2;
				}

				setgid(getegid());
				setuid(geteuid());

				conf::clnconf qhome(VQ_HOME+"/etc/ivq/qmail/qmail_home",
					"/var/qmail/");

				string fn(qhome.val_str()+qp2prog(static_cast<qmail_prog>(*av[1])));

				char * const args[2] = {
						const_cast<char *>(fn.c_str()),
						NULL
				};
				
				execv(*args, args);
				return 111;
		} catch(...) {
				return 111;
		}
}
