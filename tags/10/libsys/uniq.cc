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

#include <sys/time.h>
#include <unistd.h>

#include <stdexcept>
#include <sstream>

#include "uniq.hpp"

namespace sys {

	using std::runtime_error;
	using std::ostringstream;
	using std::string;
	
	/**
	\return unique value which will be used to create temporary files
	*/
	string uniq() {
		ostringstream u;
		struct timeval tv;
		if(gettimeofday(&tv,NULL)==-1)
				throw runtime_error("uniq: gettimeofday");
		u<<tv.tv_sec<<'.'<<tv.tv_usec<<'.'<<getpid();
		return u.str();
	}

} // namespace sys
