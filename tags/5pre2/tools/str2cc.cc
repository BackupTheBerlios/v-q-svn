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
#include "main.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int cppmain( int ac, char ** av )
{
	if( ac >= 4 ) {
			cout<<"#include \"conf_"<<av[3]<<".h\""<<endl
				<<"namespace "<<av[1]<<" {"<<endl
				<<"\tconst std::string conf_"<<av[3]<<" ( \"";
			const char *c = av[2];
			for( ; *c; ++c ) {
					if( ac < 4 && *c == '\n' ) continue;
					cout<< "\\x" << hex << (int) *c;
			}
			cout<<"\" );"<<endl
				<<"}"<<endl;
			cout.flush();

			if( ! cout ) {
					return 1;
			}
	} else {
			cerr << "usage: " << *av << " namespace string variable's_name [something]" << endl;
			return 2;
	}
	return 0;
}

