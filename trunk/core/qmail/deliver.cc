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
#include "split.h"
#include "vq_conf.h"

#include <sys/types.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <cerrno>
#include <cerrno>
#include <string>

using namespace std;
using namespace vq;
/*
 *
 */
int cppmain(int ac, char **av)
{
	char *args[11], *host, *tmp;
	string ql = ac_qmail.val_str()+"/bin/qmail-local";
	args[0] = (char*) ql.c_str();
	args[1] = "--";
	args[2] = getenv("USER");
	if( ! args[2] ) {
			cout<<"environment variable USER is not set"<<endl;
			return 111;
	}
	args[6] = getenv("EXT");
	if( ! args[6] ) {
			cout<<"environment variable EXT is not set"<<endl;
			return 111;
	}
	string ext(args[6]);
	ext = ext.substr(0,ext.find('-'));
	
	if( (tmp = getenv("HOME")) == NULL ) {
			cout<<"environment variable HOME is not set"<<endl;
			return 111;
	}
	string home(tmp);
	if( chdir(home.c_str()) ) {
			cout<<"chdir: " << home << ": " << strerror(errno) << endl;
			return 111;
	}
	if( chdir(split_user(ext).c_str()) ) {
			cout<<"deliver: Sorry, no mailbox here by that name. (#5.1.1)"<<endl;
			return 100;
	}
	home+='/';
	home+=split_user(ext);
	home+='/';
	args[3] = (char*) home.c_str();

	args[4] = getenv("LOCAL");
	if(! args[4]) {
			cout<<"environment variable LOCAL is not set"<<endl;
			return 111;
	}
	args[5] = "-";
	args[7] = host = getenv("HOST");
	if(! args[7]) {
			cout<<"environment variable HOST is not set"<<endl;
			return 111;
	}
	args[8] = getenv("SENDER");
	if( !args[8] ) {
			cout<<"environment variable SENDER is not set"<<endl;
			return 111;
	}
	args[9] = "./Maildir/";
	args[10] = 0; 
   
	execv(args[0],args);
	cout<<"can't execute "<< ql << ": "<<strerror(errno)<<endl;
	return 111;
}
