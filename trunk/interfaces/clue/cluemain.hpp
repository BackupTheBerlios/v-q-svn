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
#ifndef __CLUEMAIN_HPP
#define __CLUEMAIN_HPP

#include <string>

#include <vq.hpp>

#include <coss/CosNaming.h>

#define corba_catch(x) catch( CORBA::Exception & e ) { \
		std::cerr<<"CORBA exception"<< (x) <<": "; \
		e._print(std::cerr); \
		std::cerr<<std::endl; \
		return 10; \
}

struct cluemain_env {
	vq::ivq_var vq; //!< V-q's main object
	CosNaming::NamingContext_var ns; //!< NameService
};

int cluemain( int, char **, cluemain_env & );

#endif // ifndef __CLUEMAIN_HPP
