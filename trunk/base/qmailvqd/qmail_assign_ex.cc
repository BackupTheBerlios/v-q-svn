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
#include <pfstream.hpp>
#include <conf.hpp>
#include <vqmain.hpp>

#include <iostream>
#include <cerrno>
#include <string>

using std::string;
using std::cerr;
using std::endl;
using posix::ipfstream;

int vqmain( int ac , char ** av ) {
		try {
				if( ac != 2 ) {
						cerr<<"usage: "<<*av<<" domain_name"<<endl
							<<"Program returns 0 if specified domain_name is in qmail/users/assign,"<<endl
							<<"1 if not, anything else on error."<<endl;
						return 2;
				}

				conf::clnconf qhome(VQ_HOME+"/etc/ivq/qmail/qmail_home",
					QMAIL_HOME );
				
				ipfstream in((qhome.val_str()+"/users/assign").c_str());
				if( ! in ) {
						if( errno == ENOENT ) return 1;
						return 3;
				}

				string ln, dot(".");
				string::size_type ln_len, doml = strlen(av[1]);
				if(! doml) return 4;

				while(getline(in, ln)) {
						ln_len = ln.length();
						if( ! ln_len ) continue;
						if( ln == dot ) break;
						if( ln[0] == '+' ) {
								if( ln_len > doml+2 ) {
										if( ln[doml+1] == '-' ) {
												if( ! memcmp(ln.data()+1, av[1], doml) )
														return 0;
										}
								}
						}
				}
				if( in.bad() ) return 5;
				return 1;
		} catch(...) {
				return 111;
		}
}
