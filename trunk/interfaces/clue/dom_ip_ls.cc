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
#include "cvq.hpp"
#include "main.hpp"

#include <unistd.h>

#include <exception>
#include <iostream>
#include <cerrno>
#include <vector>

using namespace std;

const char *me = NULL;
/*
 *
 */
void usage()
{
	cerr<<"usage: "<<me<< " [-a] [domain ...]"<<endl
		<<"-a\tall domains"<<endl
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
int cppmain(int ac, char **av)
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
