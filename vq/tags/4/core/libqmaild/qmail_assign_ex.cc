/*
Copyright (c) 2003,2004 Pawel Niewiadomski
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
#include "pfstream.h"
#include "vq_conf.h"
#include "main.h"

#include <iostream>
#include <cerrno>
#include <string>

using std::string;
using std::cerr;
using std::endl;
using posix::ipfstream;
using namespace vq;

int cppmain( int ac , char ** av ) {
		try {
				if( ac != 2 ) {
						cerr<<"usage: "<<*av<<" domain_name"<<endl
							<<"Program returns 0 if specified domain_name is in qmail/users/assign,"<<endl
							<<"1 if not, anything else on error."<<endl;
						return 2;
				}
				ipfstream in((ac_qmail.val_str()+"/users/assign").c_str());
				if( ! in ) {
						if( errno == ENOENT ) return 1;
						return 3;
				}

				string ln, dot(".");
				string::size_type ln_len;
				int doml = strlen(av[1]);
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
