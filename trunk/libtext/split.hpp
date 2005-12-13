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

#ifndef __SPLIT_HPP
#define __SPLIT_HPP

#include "common.hpp"

#include <deque>
#include <string>
#include <algorithm>
#include <functional>

namespace text {

	LIBTEXT_API std::string split_path(const std::string &, const std::string &, 
		const std::string &, 
		std::string::size_type, bool rev = false );

	LIBTEXT_API std::string split_path(const std::string &, std::string::size_type, 
		const std::string &, 
		std::string::size_type, bool rev = false );
		
	LIBTEXT_API std::string split_dom(const std::string &, 
		std::string::size_type, const std::string & = "/" );

	LIBTEXT_API std::string split_id(const std::string &, 
		std::string::size_type );

	LIBTEXT_API std::string split_user(const std::string &, 
		std::string::size_type);

	struct LIBTEXT_API split_t : public std::binary_function< std::string, std::string, 
				std::deque<std::string> > {
			
			result_type operator() ( const first_argument_type &,
					const second_argument_type & ) const;
	};

	LIBTEXT_API inline split_t::result_type split( const split_t::first_argument_type & a1,
			const split_t::second_argument_type & a2 ) {
		return split_t()(a1, a2);
	}

} // namespace text

#endif // ifndef __SPLIT_HPP
