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
#include "cdaemonvq.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <sstream>
#include <cerrno>

using namespace std;
using namespace vq;

cvq * cvq::alloc() {
	return new cdaemonvq();
}

/**
 *
 */
cdaemonvq::open_error::open_error(const char * fn)
		: msg(fn), fn(fn), err_sys(errno) {
	msg+=": ";
	msg+=strerror(err_sys);
}

/**
 * \exception runtime_error if it can't create socket
 */
cdaemonauth::cdaemonauth()
{
	int curdirfd = open(".",O_RDONLY);
	if(curdirfd==-1) 
			throw open_error(conf_home.c_str());
	try {
			if(!sig_pipe_ign())
					throw sysfun_error("sig_pipe_ign");
			if(chdir((conf_home+"/sockets").c_str()))
					throw chdir_error((conf_home+"/sockets").c_str());
			if( (sock=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
					throw sysfun_error("socket");
			sock_un.sun_family = AF_UNIX;
			strcpy(sock_un.sun_path, "daemonauth");
			if(connect(sock, (struct sockaddr*) &sock_un, SUN_LEN(&sock_un)))
					throw sysfun_error("connect");
			if( fchdir(curdirfd) )
					throw chdir_error(conf_home.c_str());
			close(curdirfd);
	} catch(...) {
			close(curdirfd);
			throw;
	}
}

/**
*/
cdaemonvq::~cdaemonvq()
{
	close(sock);
}

