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

#include <iostream>
#include <cstdlib>

std::string VQ_HOME("/var/vq/");

int cppmain( int ac, char ** av ) {
	char * ptr;
	ptr = getenv("VQ_HOME");
	if(ptr) VQ_HOME = ptr;

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
		case '-':
				i=ac;
				break;
		}
	}
	return vqmain(ac, av);
}
