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
#ifndef __CPOLL_HPP
#define __CPOLL_HPP

#include <sys/poll.h>
#include <inttypes.h>
#include <stdexcept>

namespace sys {

	/**
	 * simple wrapper for poll and struct pollfd, allows you easily add, remove
	 * struct pollfd, and poll them
	 */
	class cpoll {
			public:
					struct pollfd *fds; //!< descriptors poll
					uint32_t cnt; //!< number of descriptions in fds

					cpoll();
					~cpoll();

					void add(int , short); //!< add desc.
					void rm(int); //!< remove desc.
					void chg(int, short); //!< change poll options for desc.
					int poll(int); //!< call poll on fds
					const struct pollfd & operator [] (int) const; //!< easy access

			protected:
					uint32_t left; //!< number of structures that are allocated but unused

	}; // class cpoll

} // namespace sys

#endif // ifndef __CPOLL_HPP
