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
#include "main.hpp"
#include "cdaemonmaster.hpp"

#include <iostream>
#include <stdexcept>
#include <memory>
#include <sstream>

using namespace std;

/**
 *
 */
void usage( const char * me ) {
	cerr<<"usage: "<<me<<" so_name so_symbol socket_path [children_limit]"<<endl;
}

/**
 *
 */
int cppmain( int ac, char ** av ) {
	try {
			ios::sync_with_stdio(false);

			if( ac < 4 ) {
				usage(*av);
				return 1;
			}
		
			auto_ptr<cdaemonmaster> master(cdaemonmaster::alloc());
			if( ac >= 5 ) {
				istringstream is;
				is.str(*(av+4));
				cdaemonmaster::size_type ch_lim;
				is>>ch_lim;
				if( ! is ) {
						usage(*av);
						return 1;
				}
				master->children_limit_set( ch_lim );
			}
			master->daemon_add(*(av+1), *(av+2), *(av+3));
			master->setup();
			master->run();
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	} catch( ... ) {
			cerr<< "shit, unknown exception!!!"<<endl;
			return 2;
	}
	return 0;
}
