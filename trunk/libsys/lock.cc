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

#include "lock.hpp"
#include "auto/hasflock.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/file.h>

namespace sys {
	
	char lock_ex(int fd)
	{
#ifdef HASFLOCK
		return flock(fd, LOCK_EX); 
#else
		return lockf(fd,F_LOCK,0);
#endif
	}
	
	char lock_exnb(int fd) {
#ifdef HASFLOCK
		return flock(fd, LOCK_EX|LOCK_NB);
#else
		return lockf(fd,F_TLOCK,0);
#endif
	}
	
	char unlock(int fd)
	{
#ifdef HASFLOCK
		return flock(fd, LOCK_UN);
#else
		return lockf(fd,F_ULOCK,0);
#endif
	}

	file_mutex::file_mutex( const char * fn, bool wait ) : fd(-1) {
		this->fd = open(fn, O_RDONLY | O_NONBLOCK | O_CREAT, 0444 );
		if( this->fd == -1 ) throw open_error(fn);
		if( (! wait && lock_exnb(this->fd)) || (wait && lock_ex(this->fd)) )
				throw locked_error(fn);
	}

	file_mutex::file_mutex( const std::string & fn, bool wait ) : fd(-1) {
		this->fd = open(fn.c_str(), O_RDONLY | O_NONBLOCK | O_CREAT, 0444 );
		if( this->fd == -1 ) throw open_error(fn);
		if( (! wait && lock_exnb(this->fd)) || (wait && lock_ex(this->fd)) )
				throw locked_error(fn);
	}

	file_mutex::~file_mutex() {
		if( this->fd != -1 ) {
				unlock(this->fd);
				close(this->fd);
		}
	}

} // namespace sys
