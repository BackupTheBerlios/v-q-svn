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

#ifndef __SPLIT_HPP
#define __SPLIT_HPP

#include <deque>
#include <string>

namespace text {

	std::string split(const std::string &, const std::string &, 
		const std::string &, 
		std::string::size_type, bool rev = false );

	std::string split(const std::string &, std::string::size_type, 
		const std::string &, 
		std::string::size_type, bool rev = false );
		
	std::string split_dom(const std::string &, 
		std::string::size_type, const std::string & = "/" );

	std::string split_id(const std::string &, 
		std::string::size_type );

	std::string split_user(const std::string &, 
		std::string::size_type);

	std::deque<std::string> split( const std::string &, const std::string & );

} // namespace text

#endif // ifndef __SPLIT_HPP
