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
#include "main.hpp"

#include <exception>
#include <iostream>
//#include <locale>

namespace {
	void term() {
		try {
				std::cerr<<"Program terminated."<<std::endl;
				std::cout.flush();
				std::cerr.flush();
		} catch(...) {
		}
		exit(111);
	}

	void unexp() {
		try {
				std::cerr<<"Program terminated by unexpected exception."<<std::endl;
				std::cout.flush();
				std::cerr.flush();
		} catch(...) {
		}
		exit(111);
	}
} // namespace

int main( int ac, char ** av ) {
	std::ios::sync_with_stdio(false);
	std::set_terminate(term);
	std::set_unexpected(unexp);

	try {
/*
 * only in gcc > 3
 * 
			std::locale el("");
			std::locale::global(el);
			std::cin.imbue(el);
			std::cerr.imbue(el);
			std::clog.imbue(el);
*/
			return cppmain(ac, av);
	} catch( const std::exception & e ) {
			std::cerr<<"Exception caugth: "<<e.what()<<std::endl;
			return 111;
	}
}
