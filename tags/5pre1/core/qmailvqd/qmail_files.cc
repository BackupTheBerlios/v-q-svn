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

#include "qmail_files.hpp"

const char *qf2file( qmail_file qf ) throw (std::runtime_error) {
	// we check each value, minimaze the risk
	switch( qf ) {
	case qf_assign:
		return "users/assign";
	case qf_rcpthosts:
		return "control/rcpthosts";
	case qf_morercpthosts:
		return "control/morercpthosts";
	case qf_locals:
		return "control/locals";
	case qf_virtualdomains:
		return "control/virtualdomains";
	default:
		throw std::runtime_error("qf2file: invalid code");
	}
}