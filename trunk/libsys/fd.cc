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
#include <sys/types.h>
#include <unistd.h>

#include "fd.hpp"

namespace sys {

	using namespace std;
	
	int32_t fdread( int fd, void * b, int32_t l )
	{
		int32_t i=0, r=0;
		for( ; i<l; i+=r ) {
				r = read(fd, (char*)b+i, l-i);
				switch(r) {
				case 0:
						return i;
				case -1: 
						return -1;
				}
		}
		return l;
	}
	
	int32_t fdwrite( int fd, const void * b, int32_t l )
	{
		int32_t i=0, r=0;
		for( ; i<l; i+=r ) {
				r = write(fd, (char*)b+i, l-i);
				switch(r) {
				case 0:
						return i;
				case -1: 
						return -1;
				}
		}
		return l;
	}

} // namespace sys
