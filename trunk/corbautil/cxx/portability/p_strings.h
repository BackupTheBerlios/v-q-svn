//----------------------------------------------------------------------
// Copyright (c) 2005 foo-baz.com. All rights reserved.
// This software is provided "as is".
//
// File:	p_strings.h
//
// Description:	Common operators and functions for strings.
//----------------------------------------------------------------------

#ifndef P_STRINGS_H
#define P_STRINGS_H

namespace {
		inline CORBA::Boolean _loc_strcmp( const char * pa, const char * pa1 ) {
				if( !pa ) {
						return !pa1;
				}
				if( pa == pa1 ) return true;
				for( ; *pa && *pa1; ++pa, ++pa1 )
						if( *pa != *pa1 ) break;
				return *pa == *pa1;
		}
}
inline CORBA::Boolean operator==( const CORBA::String_var & a, 
		const CORBA::String_var & a1 ) {
	return _loc_strcmp(a, a1);
}

#if defined(P_USE_OMNIORB)

inline CORBA::Boolean operator==( const CORBA::String_member & a,
		const CORBA::String_member & a1 ) {
	return _loc_strcmp(a, a1);
}

#endif

#endif
