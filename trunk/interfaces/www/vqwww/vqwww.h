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

#ifndef __VQWWW_H
#define __VQWWW_H

#include <deque>
#include <string>

#include <cvq.h>

class cenvironment;
class cmod;

/**
 * variables stored in session
 */
struct csess_basic {
        cvq::auth_info ai; //!< informations about user
        std::string cip; //!< client's IP
};

/**
 * module's information
 */
struct mod_info {
    void (*mods_init)( std::deque< cmod * > &, cenvironment & );
};

/**
 * vqwww directories
 */
class cvqwww_dirs {
		public:
				const std::string conf; //!< name of directory containing configuration
				const std::string share; //!< name of shared files directory
				const std::string libexec; //!< ...
				const std::string include; //!< ...

				cvqwww_dirs(const std::string &);
};

#endif
