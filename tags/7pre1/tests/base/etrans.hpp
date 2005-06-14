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

#ifndef __ETRANS_HPP
#define __ETRANS_HPP

#include <auth.hpp>

#include <p_orb.h>

#include <boost/test/unit_test.hpp>

#include <sstream>

#ifdef minor
	#undef minor
#endif
#ifdef major
	#undef major
#endif

/**
 * 
 */
inline void et_vq_null_error( const vq::null_error &e ) {
	std::ostringstream os;
	os<<"Unexpected NULL value in "<<e.file<<" at "<<e.line;
	BOOST_ERROR(os.str());
}

/**
 * 
 */
inline void et_vq_except( const vq::except &e ) {
	std::ostringstream os;
	os<<"Exception: "<<e.what<<" in "<<e.file<<" at "<<e.line;
	BOOST_ERROR(os.str());
}

/**
 * 
 */
inline void et_vq_db_error( const vq::db_error &e ) {
	std::ostringstream os;
	os<<"Database exception: "<<e.what<<" in "<<e.file<<" at "<<e.line;
	BOOST_ERROR(os.str());
}

inline void et_CORBA_SystemException( const CORBA::SystemException & e ) {
	std::ostringstream os;
	os<<"CORBA system exception: ";
#if defined(MICO_VERSION)
	e._print(os);
#elif defined(OMNIORB_DIST_DATE)
	os<<"minor: "<<e.NP_minorString()<<"; completed: ";
	switch(e.completed()) {
	case CORBA::COMPLETED_YES: os<<"YES"; break;
	case CORBA::COMPLETED_NO: os<<"NO"; break;
	default: os<<"MAYBE";
	}
#else
	os<<"minor: "<<e.minor()<<"; completed: ";
	switch(e.completed()) {
	case CORBA::COMPLETED_YES: os<<"YES"; break;
	case CORBA::COMPLETED_NO: os<<"NO"; break;
	default: os<<"MAYBE";
	}
#endif
	BOOST_ERROR(os.str());
}

inline void et_CORBA_Exception( const CORBA::Exception & e ) {
	std::ostringstream os;
	os<<"CORBA exception";
#ifdef MICO_VERSION	
	os<<": ";
	e._print(os);
#endif
	BOOST_ERROR(os.str());
}

#endif
