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

#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

#include "fdstr.hpp"
#include "fd.hpp"

namespace sys {
	
	using namespace std;
	
	int fdrdstr(int fd, string & str)
	{
		string::size_type len, i;
		char c;
		str = "";
		if( fdread(fd, &len, sizeof(len)) == sizeof(len) ) {
				str.reserve(len);
				for( i=0; i<len; i++ ) {
						if( read(fd, &c, 1) != 1 ) {
								return -1;
						}
						str += c;
				}
				return len;
		}
		return -1;
	}
	
	/**
	write string as: uint32_t len, char[len] data
	@return -1 on error, len on success
	*/
	int fdwrstr(int fd, const string & str)
	{
		string::size_type len = str.length();
		if( fdwrite(fd, &len, sizeof(len)) == sizeof(len)
			&& fdwrite(fd, str.data(), len) == len ) {
				return len;
		}
		return -1;
	}

} // namespace sys
