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
