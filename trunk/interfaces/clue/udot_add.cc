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
#include "cvq.hpp"
#include "main.hpp"

#include <unistd.h>

#include <exception>
#include <iostream>
#include <cerrno>
#include <deque>
#include <sstream>

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [options] (e-mail type value){1,}"<<endl
		<<"-q\tquiet mode, only first e-mail is processed"<<endl
		<<endl
		<<"Add options assiociated with mailboxes, for example autoresponse"<<endl
		<<"message, redirection address, etc."<<endl;
}

/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			bool quiet=false;
			while( (opt=getopt(ac,av,"qf:h")) != -1 ) {
					switch(opt) {
					case 'q':
							quiet=true;
							break;
					default:		
					case '?':
					case 'h':
							usage();
							return(1);
					}
			}
			ac -= optind;
			av += optind;
			if( 1 > ac || ac % 3 ) {
					usage();
					return(1);
			}

			cvq *vq(cvq::alloc());

			uint8_t ret;
			char *atpos=NULL;

			if(quiet) ac=3;
			
			cvq::udot_info ui;
			istringstream type;
			int typeval;
			for(int i=0; i < ac; i+=3 ) {
					atpos = rindex(av[i], '@');
					if(!quiet) cout<<av[i]<<": ";
					if(atpos) {
							*atpos = '\0';
							atpos++;
							type.str(av[i+1]);
							type>>typeval;

							if(!type) {
									if( !quiet ) {
											cout<<"Invalid type: "<<av[i+1]<<endl;
											continue;
									}
									else return 1;
							}
							if( ! vq->udot_sup_is(static_cast<cvq::udot_type>(typeval)) ) {
									if( !quiet ) {
											cout<<"Type is not supported: "<<av[i+1]<<endl;
											continue;
									} else return 1;
							}
							
							ui.type = static_cast<cvq::udot_type>(typeval);
							ui.val = av[i+2];
							ret = vq->udot_add(atpos, av[i], "", ui);
							if(ret) {
									if(!quiet)
											cout<<vq->err_report()<<endl;
									else return(ret);
							} else {
									if(!quiet)
											cout<<"Dot file was changed, id.:"
												<<ui.id<<endl;
							}
					} else {
							if(!quiet) cout<<"invalid e-mail"<<endl;
							else return 1;
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
