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

#ifndef __CDAEMONMASTER_HPP
#define __CDAEMONMASTER_HPP

#include "cdaemonchild.hpp"

#include <sys.hpp>

#include <dirent.h>

#include <list>
#include <map>

/**
 * One master to rule them all ;-)
 */
class cdaemonmaster {
		public:
				struct daemon_info;
				
				typedef unsigned int size_type;
				
				virtual void children_limit_set(size_type)=0;
				virtual void daemon_add( const char *, const char *, 
						const char * ) =0;
				virtual void run() = 0;
				virtual void setup() = 0;

				static cdaemonmaster * alloc();
};

/**
 * Informations about daemon.
 */
struct cdaemonmaster::daemon_info {
	cdaemonchild * (*alloc)();
};


#endif // ifndef __CDAEMONMASTER_H
