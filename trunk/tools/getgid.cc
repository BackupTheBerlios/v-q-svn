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

#include <sys/types.h>
#include <grp.h>

#include <iostream>
#include <string>

using namespace std;

int cppmain(int ac, char **av) {
	if( ac != 2 ) {
		cerr<<"usage: " << *av<< " name"<<endl;
		return 1;
	}
		
	struct group *gr;
	gr = getgrnam(av[1]);
	if( ! gr ) {
		cerr<<"can't find group: "<<av[1]<<endl;
		return 2;
	}
	cout << gr->gr_gid << endl;  
	return 0;
}
