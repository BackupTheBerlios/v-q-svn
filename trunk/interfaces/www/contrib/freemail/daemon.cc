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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <string>

#include "daemon.h"

using namespace std;

// global data
int sso, cso;
cpoll fds;

/**
 *
 */
rd_error::rd_error() : std::runtime_error((string)"read error: "+strerror(errno)) {
}

/**
 *
 */
wr_error::wr_error() : std::runtime_error((string)"write error: "+strerror(errno)) {
}

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
	} catch( ... ) {
			cerr<< "shit, unknown exception!!!"<<endl;
			return 2;
	}
	return 0;
}
