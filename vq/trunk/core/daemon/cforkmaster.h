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

#ifndef __CFORKMASTER_H
#define __CFORKMASTER_H

#include "cdaemonmaster.h"

#include <sys.hpp>

#include <dirent.h>

#include <list>
#include <map>
#include <algorithm>
#include <exception>

/**
 * For each incoming connection fork child which will process all requests
 * from child.
 */
class cforkmaster : public cdaemonmaster {
		public:
				class fork_error;
				
				cforkmaster();
				virtual ~cforkmaster();

				virtual void children_limit_set(size_type);

				virtual void daemon_add( const char *, const char *, 
						const char * );

				virtual void run();
				virtual void setup();

		protected:
				struct dll_info;
				
				typedef std::map< int, dll_info > socket2dll_map;
				
				static size_type children; //!< number of children
				static size_type children_lim; //!< maximum of children
						
				sys::cpoll fds; //!< file descriptor we're listening on
				DIR * dir_initial; //!< directory from which we have been run
				socket2dll_map sock2dll; //!< map sockets to dll-s
				bool set; //!< true if setup was executed

				void mod_load( dll_info & , const char *, const char * );

				void child_accept( int );
				void children_dump();

				static void wait_run( int );
};

/**
 *
 */
class cforkmaster::fork_error : public std::exception {
		public:
				virtual ~fork_error() throw() {}
				
				const char * what() const throw();
};

/**
 * Informations about plug-in
 */
struct cforkmaster::dll_info {
		void * dll;
		daemon_info * di;
		int lock;
};

#endif // ifndef __CFORKMASTER_H
