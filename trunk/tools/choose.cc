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
#include "main.h"

#include <string>
#include <iostream>

using namespace std;

int cppmain( int ac, char **av )
{
	ac--;
	av++;
	for( int i=1; i<ac; i+=2 ) {
		if( ! strcmp(av[0],av[i]) ) {
				cout<<av[i+1];
				return 0;
		}
	}
	cerr<<av[0]<<" does not match anything"<<endl;
	return 1;
}
