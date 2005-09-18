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
#include "vqmain.hpp"

#include <main.hpp>
#include <vq.hpp>

#include <iostream>
#include <cstdlib>

std::string VQ_HOME;
std::string VQ_ETC_DIR;

int cppmain( int ac, char ** av ) try {
	char * ptr;
	ptr = getenv("VQ_HOME");
	VQ_HOME = ptr ? ptr : "/var/vq/";

	ptr = getenv("VQ_ETC_DIR");
	VQ_ETC_DIR = ptr ? ptr : (VQ_HOME+"/etc");
	
	for( int i=1; i<ac; ++i ) {
		if( '-' != *av[i] ) continue;
		switch( *(av[i]+1) ) {
		case 'H':
				if( i+1 == ac ) {
						std::cerr<<"missing argument for -H option"<<std::endl;
						return 1;
				}
				VQ_HOME = av[i+1];
				break;
		case 'E':
				if( i+1 == ac ) {
						std::cerr<<"missing argument for -E option"<<std::endl;
						return 1;
				}
				VQ_ETC_DIR = av[i+1];
				break;
		case '-':
				i=ac;
				break;
		}
	}
	return vqmain(ac, av);
} catch( vq::except & e ) {
	std::cerr<<"Exception: "<<e.what<<" in "<<e.file<<" at "<<e.line<<std::endl;
	return 111;
} catch( vq::db_error & e ) {
	std::cerr<<"Database exception: "<<e.what<<" in "<<e.file<<" at "<<e.line<<std::endl;
	return 111;
} catch( vq::null_error & e ) {
	std::cerr<<"Unexpected NULL value in "<<e.file<<" at "<<e.line<<std::endl;
	return 111;
} catch( CORBA::SystemException & e ) {
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
	return 111;
} catch( CORBA::Exception & e ) {
	std::cerr<<"CORBA exception";
#ifdef MICO_VERSION	
	std::cerr<<": ";
	e._print(std::cerr);
#endif
	std::cerr<<std::endl;
	return 111;
}
