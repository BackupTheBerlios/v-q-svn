/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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

#ifndef __UTIL_HPP
#define __UTIL_HPP

#include <inttypes.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <deque>

namespace sys {

	/// write uint32_t to file
	bool dumpuint32_t(const std::string &, uint32_t);
	
	/// write std::string (including \0) to file
	bool dumpstring0( const std::string &, const std::string & );
	
	/// write std::string to file
	bool dumpstring( const std::string &, const std::string & );
	
	bool dumpu32str( const std::string &, const std::string & );
	
	bool getu32str( const std::string &, std::string & );
	
	/// unlink files
	bool unlink( const std::deque<std::string> & );
	
	/// touch files
	bool touch( const std::deque<std::string> &, bool=false );
	
	/// replace all .,- to _
	std::string str2tb(const std::string &);
	
	/// put \ before \, ", '
	std::string escape(const std::string &);
	void escape(std::string &);
	
} // namespace sys

#endif // ifndef __UTIL_HPP
