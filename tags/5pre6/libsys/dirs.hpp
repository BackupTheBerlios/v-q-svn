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

#ifndef __DIRS_HPP
#define __DIRS_HPP

#include <sys/types.h>
#include <stdexcept>
#include <string>

namespace sys {

	bool mkdirhier( const char *, mode_t );
	bool mkdirhier( const char *, mode_t, int, int );
	bool rmdirrec( const std::string & n );

} // namespace sys

#endif // ifndef __DIRS_HPP
