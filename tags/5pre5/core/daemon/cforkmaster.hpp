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

#ifndef __CFORKMASTER_HPP
#define __CFORKMASTER_HPP

#include "cdaemonmaster.hpp"

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
