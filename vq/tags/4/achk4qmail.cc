/*
Copyright (c) 2002 Pawel Niewiadomski
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
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sem.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>

#include "vq_conf.h"
#include "vq_init.h"
#include "sig.h"
#include "fd.h"
#include "fdstr.h"
//#include "util.h"
#include "lock.h"
#include "sys.h"

using namespace std;

// socket
string fnlock = "achk_socket.lock";
string dir = conf_qmail+"/control";
string fn = "achk_socket";
int slock, sso, cso;
// child
bool child_we, child_re;
char addr_type=0;
string addr, user, dom, tb;
// VQ
cvq *vq = NULL;
cpoll fds;

/**
@exception runtime_error if it can't create socket
*/
void setup()
{
	umask(0);
			
	if(!sig_pipeign() || !sig_chld_nocldwait()) 
			throw runtime_error((string)"setup: can't set signals: "+strerror(errno));
	if(chdir(dir.c_str()))
			throw runtime_error((string)"setup: chdir: "
					+dir+": "+strerror(errno));
	slock = open(fnlock.c_str(), O_RDONLY | O_NONBLOCK | O_CREAT, 0600 );
	if(slock==-1)
			throw runtime_error((string)"setup: open: "+fnlock+": "+strerror(errno));
	if(lock_exnb(slock)) {
			if( errno == EWOULDBLOCK )
					throw runtime_error((string)"setup: "+fnlock+" is already locked");
			else throw runtime_error((string)"setup: "+fnlock+": lock: "+strerror(errno));
	}
	if(unlink(fn.c_str())==-1 && errno != ENOENT )
			throw runtime_error((string)"setup: unlink: "+fn+": "+strerror(errno));
	if( (sso=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
			throw runtime_error((string)"setup: socket: "+strerror(errno));

	struct sockaddr_un sun;
	sun.sun_family = AF_UNIX;
	strncpy(sun.sun_path, fn.c_str(), sizeof(sun.sun_path)-1);
	if(bind(sso, (struct sockaddr*) &sun, SUN_LEN(&sun)))
			throw runtime_error((string)"setup: bind: "+fn+": "+strerror(errno));
	if(chmod(fn.c_str(), 0600))
			throw runtime_error((string)"setup: chmod: "+fn+": "+strerror(errno));
	if( listen(sso, 5) == -1 )
			throw runtime_error((string)"setup: listen: "+strerror(errno));
}

/**
 *
 */
void ue() {
	uint8_t ret = vq->user_ex(dom, user);
	cout<<(int)ret<<endl;
	if( 1 == ret ) {
			if(fdwrite(cso, &"Z", 1) != 1
			   || fdwrstr(cso,"553 no such mailbox; nie ma takiej skrzynki") == -1 ) child_we=true;
			return;
	}
	if( 2 == ret) {
			if( 1 != fdwrite(cso, &"K", 1) ) child_we = true;
			return;
	}
			if( 1 != fdwrite(cso, &"K", 1) ) child_we = true;
}

/**
 *
 */
void achk() {
	string::size_type atpos;
	if( fdrdstr(cso, addr) != -1 ) {
			if('M' == addr_type 
			   && (addr.empty() || (1==addr.length() && '\0'==addr[0])) ) {
					if(fdwrite(cso, &"K",1) != 1) child_we=true;
					return;
			}
			atpos = addr.find('@');
			if( string::npos == atpos ) {
					if(fdwrite(cso, &"Z", 1) != 1
					   || fdwrstr(cso,"553 sorry, I accept addresses only with @ sign; przykro mi, ale adres musi zawierac znak @") == -1 ) child_we=true;
					return;
			}
			dom = addr.substr( atpos+1 );
			user = addr.substr( 0, atpos );
			if(dom.empty() || user.empty()) {
					if(fdwrite(cso, &"Z", 1) != 1
					   || fdwrstr(cso,"553 invalid e-mail address; nieprawidlowy adres e-mail") == -1 ) child_we=true;
					return;
			}
			ue();
	} else child_re = true;
}

/**
*/
void (*cmd_proc( char cmd )) () {
	addr_type = 0;
	switch(cmd) {
	case 'M':
			if(!addr_type) addr_type = 'M';
	case 'R':
			if(!addr_type) addr_type = 'R';
			return &achk;
	default:
		return NULL;
	}
}
/**
*/
void child()
{
	char cmd;
	void (*run)();
	
	child_we = child_re = false;
	switch( fdread(cso, &cmd, 1) ) {
	case 0: return;
	case -1: 
			child_re = true;
			goto child_end;
	}
	// process cmd
	if( ! (run = cmd_proc(cmd)) ) {
			cerr << "child: unknown command: " << (int) cmd << endl;
			return;
	}
child_end:
	if( child_re ) {
			cerr << "child: read error: " << strerror(errno) << endl;
	}
	if( child_we ) {
			cerr << "child: write error: " << strerror(errno) << endl;
	}
}

/**
*/
int main()
{
	try {

	struct sockaddr_un cun;
	socklen_t cunl;
			
	setup();
	vq_init(&vq, &auth);
	
	fds.add(sso, POLLIN | POLLERR | POLLNVAL );
	for( ;; ) {
			if( fds.poll(-1) > 0 ) {
					for( i=1; i < fds.cnt; i++ ) {
							if( ! fds[i].revents ) continue;
							if( fds[i].revents & POLLNVAL
								|| fds[i].revents & POLLHUP 
								|| fds[i].revents & POLLERR ) {
									close(fds[i].fd);
									fds.rm(fds[i].fd);
									continue;
							}
							if( fds[i].revents & POLLIN ) {
									cso = fds[i].fd;
									child();
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
							cerr<<"socket problem: "<< fds[0].revents <<": "<<strerror(errno)<<endl;
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
