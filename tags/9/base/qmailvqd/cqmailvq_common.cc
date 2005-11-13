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
#include "cqmailvq_common.hpp"

#include <p_orb.h>

// against minor(x) from sysmacros.h
#ifdef minor
#undef minor
#endif

namespace vq {

	void report_exception( CORBA::SystemException & e ) {
		std::cerr<<"CORBA system exception: ";
#if defined(MICO_VERSION)
		e._print(std::cerr);
#elif defined(OMNIORB_DIST_DATE)
		std::cerr<<"minor: "<<e.NP_minorString()<<"; completed: ";
		switch(e.completed()) {
		case CORBA::COMPLETED_YES: std::cerr<<"YES"; break;
		case CORBA::COMPLETED_NO: std::cerr<<"NO"; break;
		default: std::cerr<<"MAYBE";
		}
#else
		std::cerr<<"minor: "<<e.minor()<<"; completed: ";
		switch(e.completed()) {
		case CORBA::COMPLETED_YES: std::cerr<<"YES"; break;
		case CORBA::COMPLETED_NO: std::cerr<<"NO"; break;
		default: std::cerr<<"MAYBE";
		}
#endif
		std::cerr<<std::endl;
	}

	void report_exception( CORBA::Exception & e ) {
		std::cerr<<"CORBA exception";
#ifdef MICO_VERSION	
		std::cerr<<": ";
		e._print(std::cerr);
#endif
		std::cerr<<std::endl;
	}
} // namespace vq
