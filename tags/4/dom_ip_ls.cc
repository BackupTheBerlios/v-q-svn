/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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

#include <exception>
#include <iostream>
#include <cerrno>
#include <unistd.h>
#include <vector>

#include "cvq.h"

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-a] (domain){1,}"<<endl
		<<"\t-a\tall domains"<<endl
		<<endl
		<<"Program shows IP addresses assiociated with given domains'"<<endl
		<<"names."<<endl;
}

/*
 *
 */
void show( const string & dom, const vector<string> & ips ) 
{
	vector<string>::size_type i, cnt=ips.size();
	for(i=0; i < cnt; i++ )
			cout<<dom<<':'<<ips[i]<<endl;
}

/*
 *
 */
int main(int ac, char **av)
{
	me = *av;
	try {
			int opt;
			bool all=false;
			while( (opt=getopt(ac,av,"ah")) != -1 ) {
					switch(opt) {
					case 'a':
							all = true;
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

			cvq *vq(cvq::alloc());

			char ret;
			vector<string> ips;
			vector<string> dom;
			if( all ) {
					if(ac) {
							usage();
							return(1);
					}
					if((ret=vq->dom_ip_ls_dom(dom))) {
							cout<<"Can't get domain list: "<<vq->err_report()<<endl;
							return ret;
					}
			} else {
					if(!ac) {
							usage();
							return(1);
					}
					dom.reserve(ac);
					for(int i=0; i < ac; i++ ) {
							dom.push_back(av[i]);
					}
			}
			for(vector<string>::size_type i=0, s=dom.size(); i < s; i++ ) {
					if((ret=vq->dom_ip_ls(dom[i], ips))) {
							cout<<"Can't get ip list: "<<vq->err_report()<<endl;
							return ret;
					} else {
							show(dom[i], ips);
					}
			}
	} catch( const exception & e ) {
			cerr << "exception: " << e.what() << endl;
			return 1;
	}
	return 0;
}
