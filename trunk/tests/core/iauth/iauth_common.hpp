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
#ifndef __IAUTH_COMMON_HPP
#define __IAUTH_COMMON_HPP

#define std_try { try
#define std_catch catch( vq::null_error & e ) { \
	BOOST_ERROR("null_error"); \
} catch( vq::db_error & e ) { \
	BOOST_ERROR(e.what); \
} catch( vq::except & e ) { \
	BOOST_ERROR(e.what); \
} }
	
#define IVQ_ERROR_EQUAL(eptr, ecmp) \
do { \
	::vq::ivq::error_var ev(eptr); \
	::vq::ivq::err_code ec = ecmp; \
	BOOST_CHECK_EQUAL(ev->ec, ec); \
	if( ec != ev->ec ) \
		BOOST_ERROR(error2str(ev)); \
} while(0)

#endif // ifndef __IAUTH_COMMON_HPP
