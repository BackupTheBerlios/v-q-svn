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
#include "cvq.h"
#include "main.h"

#include <unistd.h>

#include <exception>
#include <iostream>
#include <cerrno>

using namespace std;

static const char *me = NULL;
static bool quiet = false;
static cvq *vq = NULL;

/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-q] (e-mail){1,}"<<endl
		<<"\t-q\tquiet mode"<<endl
		<<endl
		<<"Delete users. It copies all files from users's mailbox to delete"<<endl
		<<"directory in home dir. So it's possible to restore user's messages."<<endl;
}

bool user_rm(const string &e)
{
	string u, d;
	string::size_type atpos;
	if(!quiet) cout<<e<<": ";
	if( (atpos=e.find('@')) == string::npos
		|| (u = e.substr(0,atpos)).empty()
		|| (d = e.substr(atpos+1)).empty() ) {
			if(!quiet)
					cout<<"invalid e-mail"<<endl;
			return quiet;
	}
	char ret;
	if((ret=vq->user_rm(u,d))) {
			if(!quiet)
					cout<<vq->err_report()<<endl;
			return quiet;
	} else {
			if(!quiet)
					cout<<"user was removed."<<endl;
	}
	return 0;
}
/*
 *
 */
int cppmain(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			while( (opt=getopt(ac,av,"rqh")) != -1 ) {
					switch(opt) {
					case 'q':
							quiet = true;
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
			if( ! ac ) {
					usage();
					return(1);
			}

			vq = cvq::alloc();

			if(quiet) ac = 1;
			for(int i=0; i < ac; i++ ) {
					if( user_rm(av[i]) )
							return 1;
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
