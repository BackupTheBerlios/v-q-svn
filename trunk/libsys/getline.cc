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

#include <unistd.h>

#include "getline.hpp"

namespace sys {
	
	using namespace std;
	
	/**
	 * read line from input
	 */
	bool getline( int fd, string & b, char delim )
	{
		b = "";
		char c;
		for(;;) {
				switch(read(fd,&c,1)) {
				case 1:
					if(c==delim)
							return true;
					b+=c;
					break;
				case 0: 
				case -1:
					return false;
				}
		}
	}

} // namespace sys

