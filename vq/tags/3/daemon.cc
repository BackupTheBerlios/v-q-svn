/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <cerrno>

#include "daemon.h"

using namespace std;

// external functions
char child(int);
void setup() throw (runtime_error);

// global data
int sso, cso;
cpoll fds;

/**
*/
int main()
{
	try {
	
	struct timeval stimeo = { 1, 0 };
	struct sockaddr_un cun;
	socklen_t cunl;
	uint32_t i;
	int fd_new;

	ios::sync_with_stdio(false);

	setup();

	fds.add(sso, POLLIN | POLLERR | POLLNVAL );
	for( ;; ) {
			if( fds.poll(-1) > 0 ) {
					for( i=1; i < fds.cnt; i++ ) {
							if( ! fds[i].revents ) continue;
							if( fds[i].revents & POLLNVAL 
								|| fds[i].revents & POLLHUP 
								|| fds[i].revents & POLLERR
								|| (fds[i].revents & POLLIN && child(fds[i].fd))) {
									close(fds[i].fd);
									fds.rm(fds[i].fd);
							}
					}
					if( fds.fds[0].revents & POLLIN ) {
							cunl = sizeof(cun);
							if( (fd_new=accept(sso, (struct sockaddr *)&cun, &cunl)) != -1 ) {
									if( setsockopt(fd_new, SOL_SOCKET, SO_SNDTIMEO, 
										& stimeo, sizeof(stimeo))
										|| setsockopt(fd_new, SOL_SOCKET, SO_RCVTIMEO,
										& stimeo, sizeof(stimeo)) )
											cerr << "setsockopt: " << strerror(errno) << endl;
									fds.add(fd_new, POLLIN | POLLERR | POLLNVAL | POLLHUP );
							} else cerr<< "accept: " << strerror(errno) << endl;
					} else if( fds[0].revents ) {
							cerr<<"socket problem: "<< fds[0].revents << ": "<<strerror(errno)<<endl;
							return 1;
					}
			} else cerr<< "poll: " << strerror(errno) << endl;
	}

	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
