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

#include "cforkmaster.hpp"

#include <sys.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <signal.h>

#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <string>
#include <algorithm>

#ifndef RTLD_NOW
	#define RTLD_NOW 0
#endif 

using namespace std;
using namespace sys;

cforkmaster::size_type cforkmaster::children(0);
cforkmaster::size_type cforkmaster::children_lim(50); 

/**
 *
 */
cdaemonmaster * cdaemonmaster::alloc() {
	return new cforkmaster();
}

/**
 *
 */
const char *cforkmaster::fork_error::what() const throw() {
	return strerror(errno);
}

/**
 * 
 */
cforkmaster::~cforkmaster() {
	socket2dll_map::iterator beg, end;
	for( beg=sock2dll.begin(), end=sock2dll.end(); beg!=end; ++beg ) {
			fds.rm(beg->first);
			close(beg->first); // close socket
			close(beg->second.lock); // close lock file
			dlclose(beg->second.dll); // close dll handle
	}
	if( dir_initial ) closedir(dir_initial);
}
/**
 *
 */
cforkmaster::cforkmaster() 
		: dir_initial(NULL), set(false) {
	string err_cwd_opendir("can't open current directory: ");
	
	if( ! (dir_initial = opendir(".")) )
			throw runtime_error(err_cwd_opendir+strerror(errno));
}

/**
 * Initialize all daemons
 */
void cforkmaster::setup() {
	string err_sig_set("can't set signals: ");
	if( !sig_pipe_ign() || !sig_child( &cforkmaster::wait_run ) ) 
			throw runtime_error(err_sig_set+strerror(errno));

	if( ! sig_block(SIGCHLD) )
			throw runtime_error((string)"can't block SIGCHLD: "+strerror(errno));
	set = true;
}

/**
 * Load module and add entry in sock2dll map pointing socket to dll.
 * \param sock socket opened for this module
 * \param so_name shared library name
 * \param so_sym daemon_info symbol name
 */
void cforkmaster::mod_load( dll_info & di, const char * so_name, 
		const char * so_sym ) {

	if( ! (di.dll = dlopen(so_name, RTLD_NOW)) )
			throw runtime_error((string)"dlopen: "+so_name+": "+dlerror());
	
	di.di = (daemon_info *) dlsym(di.dll, so_sym);
	if( ! di.di )
			throw runtime_error((string)"dlsym: "+so_name+": "+so_sym+": "+dlerror());
}

/**
 * \param so_name shared object filename
 * \param so_sym shared object symbol
 * \param sock_path socket path
 */
void cforkmaster::daemon_add( const char * so_name, const char * so_sym,
		const char * sock_path ) {
	
	struct sockaddr_un sun;

	char *sock_path_slash = rindex(sock_path, '/');
	string sock_dir( sock_path, 
		! sock_path_slash ? 0 : distance<const char *>(sock_path, sock_path_slash) );

	string sock_file( ! sock_path_slash ? sock_path : sock_path_slash + 1 );
	if( sock_file.length() > sizeof(sun.sun_path))
		sock_file.erase(sizeof(sun.sun_path));
	
	string sock_file_lock( sock_file + ".lock" );

	if( ! sock_dir.empty() && chdir(sock_dir.c_str()) )
			throw runtime_error("chdir: "+sock_dir+": "+strerror(errno));

	dll_info daemon;
	daemon.lock = open(sock_file_lock.c_str(), O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
	if(daemon.lock==-1)
			throw runtime_error("open: "+sock_file_lock+": "+strerror(errno));
	
	if(lock_exnb(daemon.lock)) {
			if( errno == EWOULDBLOCK )
					throw runtime_error(sock_file_lock+": is already locked");
			else 
					throw runtime_error(sock_file_lock+": lock: "+strerror(errno));
	}

	if(unlink(sock_file.c_str())==-1 && errno != ENOENT )
			throw runtime_error("unlink: "+sock_file+": "+strerror(errno));

	int sock;
	if( (sock=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
			throw runtime_error((string)"socket: "+strerror(errno));

	sun.sun_family = AF_UNIX;
	strncpy(sun.sun_path, sock_file.c_str(), sizeof(sun.sun_path));

	if(bind(sock, (struct sockaddr*) &sun, SUN_LEN(&sun)))
			throw runtime_error("bind: "+sock_file+": "+strerror(errno));
	if( listen(sock, 5) == -1 )
			throw runtime_error((string)"listen: "+strerror(errno));
	
	if( fchdir(dirfd(dir_initial)) )
			throw runtime_error("can't chdir back to initial directory!!!");
	
	mod_load(daemon, so_name, so_sym);
	sock2dll[ sock ] = daemon;
	fds.add(sock, POLLIN | POLLERR | POLLNVAL);
}

/**
 *
 */
void cforkmaster::run() {
	if( ! set ) throw logic_error("you didn't run setup!");

	uint32_t i;

	for( ;; ) {
			if( !sig_unblock(SIGCHLD) )
					throw runtime_error((string)"can't unblock SIGCHLD: "+strerror(errno));

			if( fds.poll(-1) <= 0 ) {
					if( errno != EINTR )
							cerr<< "poll: " << strerror(errno) << endl;
					continue;
			}
			
			for( i=0; i < fds.cnt; ++i ) {
					if( ! fds[i].revents ) continue;

					if( fds[i].revents & POLLIN ) {
							while( children >= children_lim ) {
									sig_pause();
									children_dump();
							}
							try {
									child_accept(fds[i].fd);
							} catch( fork_error & e ) {
									cerr<<"fork failed: "<<e.what()<<endl;
							}
					} else if( fds[i].revents ) {
							cerr<<"socket #"<< fds[i].fd 
								<<": "<< fds[i].revents 
								<< ": "<<strerror(errno)<<endl;
					}
			}
	}
}

/**
 * Fork and let the child process requests
 * \param fd_parent main socket
 * \exception fork_error indicates that fork failed
 */
void cforkmaster::child_accept( int fd_parent ) {
	static struct sockaddr_un cun;
	static socklen_t cunl;
	int fd_child;

	cunl = sizeof(cun);
	
	if( (fd_child=accept(fd_parent, (struct sockaddr *)&cun, &cunl)) == -1 ) {
			cerr<< "accept on #"<< fd_parent <<": " << strerror(errno) << endl;
			return;
	}
	if( !sig_block(SIGCHLD) )
			throw runtime_error((string)"can't block SIGCHLD: "+strerror(errno));

	++children;
	children_dump();

	switch(fork()) {
	case -1: 
			--children;
			children_dump();
			throw fork_error();
	case 0: 
			break;
	default: 
			close(fd_child); 
			return;
	}

	int estat = 111;
	try {
			close(fd_parent); // close main fd, we don't need it
			
			cdaemonchild *dch((*sock2dll[fd_parent].di->alloc)());
			dch->setup();

			bool loop;
			for( loop = true; loop; ) {
					switch(dch->child(fd_child)) {
					case 0: 
							break;
					case 1: 
							estat = 0;
							loop = false;
							break;
					default: 
							estat = 100;
							loop = false;
					}
			}
	} catch( exception & e ) {
			cerr<<"child_accept: exception: "<<e.what()<<endl;
	}
	cerr.flush();
	cout.flush();
	_exit(estat);
}

/**
 * Run wait to get rid of zombies
 */
void cforkmaster::wait_run( int ) {
	int stat=0;
	bool loop;
	for( loop = true; loop; ) {
			switch(waitpid(-1, &stat, WNOHANG) ) {
			case 0: case -1: loop = false; break;
			default:
					if( children ) --children;
					if( WIFEXITED(stat) ) {
							if( WEXITSTATUS(stat) )
									cerr<<"child exited with: "
										<<WEXITSTATUS(stat)<<endl;
					} else if( WIFSIGNALED(stat) ) {
							cerr<<"child got signal: "<<WTERMSIG(stat)<<endl;
					}
			}
	}
}

/**
 * Set limit of children that can be forked
 */
void cforkmaster::children_limit_set(size_type chl) {
	children_lim = chl;
}

/**
 * Print some statistics
 */
void cforkmaster::children_dump() {
	cout<<"status: "<<children<<'/'<<children_lim<<endl;
}

