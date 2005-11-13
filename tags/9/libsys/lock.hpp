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

#ifndef __LOCK_HPP
#define __LOCK_HPP

#include <stdexcept>

namespace sys {

	char lock_ex( int );
	char lock_exnb( int );
	char unlock( int );

	/**
	 * Simple file-based locking mechanism
	 */
	class file_mutex {
			public:
					class locked_error : public std::runtime_error {
							public:
									locked_error( const char * fn ) throw() 
										: std::runtime_error(fn) {}
									locked_error( const std::string & fn ) throw()
										: std::runtime_error(fn) {}
									virtual ~locked_error() throw() {}
					};

					class open_error : public std::runtime_error {
							public:
									open_error( const char * fn ) throw() 
										: std::runtime_error(fn) {}
									open_error( const std::string & fn ) throw() 
										: std::runtime_error(fn) {}
									virtual ~open_error() throw() {}
					};

					/**
					 * @exception locked_error when lock is not possible
					 * @exception open_error when can't open file
					 * @param fn file name
					 * @param wait wait for lock?
					 */
					//@{
					file_mutex( const char * fn, bool wait = false );
					file_mutex( const std::string & fn, bool wait = false );
					//@}
					virtual ~file_mutex();
			private:
					int fd;
	};

} // namespace sys

#endif // ifndef __LOCK_HPP
