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
#include "main.h"
#include "sys.h"
#include "cdaemonauth.h"

#include <dirent.h>

#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <algorithm>
#include <cerrno>
#include <string>
#include <fstream>
#include <memory>

using namespace std;

/**
 *
 */
void usage( const char * me ) {
	cout<<"usage: "<<me<<" number_of_children requests_limit"<<endl;
}

/**
 *
 */
void wait_run( int ) {
	int stat=0;
	bool loop;
	for( loop = true; loop; ) {
			switch(waitpid(-1, &stat, WNOHANG) ) {
			case 0: case -1: loop = false; break;
			default:
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
 *
 */
void * child_run( void * _req_lim ) {
	int req_lim = reinterpret_cast<int>(_req_lim);
	try {
			auto_ptr<cauth> auth(cauth::alloc());
			if( req_lim-- ) {
					do {
					} while( req_lim-- );
			}
	} catch( exception & e ) {
			cout<<"E"<<e.what()<<endl;
	}
	cerr.flush();
	cout.flush();
	return NULL;
}

/**
 *
 */
void children_start( int ch_lim, int req_lim ) {
	list<pthread_t> ths;
	pthread_t th;
	
	for( int i=0; i<ch_lim; ++i ) {
			if( ! pthread_create(&th, NULL, &child_run, 
						reinterpret_cast<void *>(req_lim) ) ) {
					ths.push_front(th);
			}
	}
	for( list<pthread_t>::iterator beg=ths.begin(), end=ths.end(); 
				beg!=end; ++beg) {
			pthread_join(*beg, NULL);
	}
}

/**
 *
 */
int cppmain( int ac, char ** av ) {
	if( ac < 3 ) {
			usage(*av);
			return 1;
	}
	try {
			if(!sig_child(&wait_run))
					throw runtime_error((string)"can't set handler for SIGCHLD: "+strerror(errno));
			if(!sig_pipe_ign())
					throw runtime_error((string)"can't ignore SIGPIPE: "+strerror(errno));

			int ch_lim, req_lim;

			istringstream is;
			is.str(av[1]);
			is>>ch_lim;
			if( ! is ) {
					usage(*av);
					return 1;
			}
			is.clear();
			is.str(av[2]);
			is>>req_lim;
			if( ! is ) {
					usage(*av);
					return 1;
			}

			children_start(ch_lim, req_lim);
	} catch( exception & e ) {
			cerr<<"exception: "<<e.what()<<endl;
			return 111;
	} catch( ... ) {
			cerr<<"shit, unknown error!"<<endl;
			return 111;
	}
	
	return 0; 
}
