/*
Copyright (c) 2002,2003,2004 Pawel Niewiadomski
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

#ifndef __CDAEMONCHILD_HPP
#define __CDAEMONCHILD_HPP

#include <stdexcept>
#include <string>
#include <cerrno>

/**
 * This class represents module interface used by daemon.cc
 */ 
class cdaemonchild {
		public:
				/**
				 * This exception is thrown when can't read from child.
				 */
				class rd_error : public std::runtime_error {
						public:
								inline rd_error();
				};

				/**
				 * This exception is thrown when can't write to child.
				 */
				class wr_error : public std::runtime_error {
						public:
								inline wr_error();
				};

				/**
				 * Wrong command.
				 */
				class bad_command : public std::runtime_error {
						public:
								inline bad_command( const std::string & );
				};

				virtual void setup() = 0; //!< Initialize module
				virtual int child( int ) = 0; //!< Process child requests
};

/**
 *
 */
cdaemonchild::rd_error::rd_error() 
	: std::runtime_error((std::string)"read error: "+std::strerror(errno)) {
}

/**
 *
 */
cdaemonchild::wr_error::wr_error() 
	: std::runtime_error((std::string)"write error: "+std::strerror(errno)) {
}

/**
 * 
 */
cdaemonchild::bad_command::bad_command( const std::string & cmd ) 
	: std::runtime_error("bad command: "+cmd) {
}
#endif // ifndef __CDAEMON_H
