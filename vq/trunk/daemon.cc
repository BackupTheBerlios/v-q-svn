/*
Copyright (c) 2002,2003,2004 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/
#include "main.h"
#include "cdaemonmaster.h"
#include "main.h"

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
