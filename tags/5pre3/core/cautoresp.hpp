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

#ifndef __CAUTORESP_H
#define __CAUTORESP_H

#include <string>
#include <map>

/**
 * Autoresponder's Interface.
 */
class cautoresp {
		public:
				/// Map which includes header fields and their values
				typedef std::map<std::string, std::string> map_hdr_val;

				/// Message to send
				virtual void msg_set( const std::string & )=0;

				/// Headers added to response
				virtual void hdrs_add_set( const map_hdr_val & )=0;

				/// Send auto-reply
				virtual void reply( const map_hdr_val & )=0;

				/// Codepage of response
				virtual void codepage_set( const std::string & )=0;
	
				/// defined in class'es implementation
				static cautoresp *alloc();
};

#endif