/*
Copyright (c) 2003,2004 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
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
