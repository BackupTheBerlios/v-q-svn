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


#include "cauth.hpp"

#include <cerrno>

using namespace std;

cauth::cauth() : lr_errno(0), lastret(cvq::err_no), lastret_blkd(false) {
}

/**
 *
 */
cauth::~cauth() {
}

/**
 *
 */
uint8_t cauth::lr( uint8_t ret, const std::string & msg ) { 
		if( ret != cvq::err_no && ret != lastret && ! lastret_blkd ) {
				lastret = ret;
				lr_info = msg;
				lr_errno = errno;
		}
		return ret;
}

/**
*
*/
string cauth::err_info() const {
   return lr_info.c_str();
}

/**
*
*/
int cauth::err_sys() const {
   return lr_errno;
}

/**
*
*/
string cauth::err_report() const {
   return cvq::err_str(lastret)+": "+lr_info+": "+strerror(lr_errno);
}
