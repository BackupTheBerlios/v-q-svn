/*
Copyright (c) 2002,2003,2004 Pawel Niewiadomski
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

