/*
Copyright (c) 2004 Pawel Niewiadomski
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
