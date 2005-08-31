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
#include "cpaths.hpp"

#include <vqmain.hpp>
#include <text.hpp>
#include <conf.hpp>

#include <unistd.h>

#include <exception>
#include <iostream>
#include <cerrno>
#include <cerrno>
#include <string>

using namespace std;

/*
 *
 */
int vqmain(int ac, char **av)
{
	char *args[11], *host, *tmp;

	string conf_dir(VQ_ETC_DIR+"/ivq/qmail/");
	conf::clnconf qhome(VQ_ETC_DIR+"/ivq/qmail/qmail_home", QMAIL_HOME);
	conf::cintconf split_user(conf_dir+"split_user", SPLIT_USER);
	conf::cintconf split_dom(conf_dir+"split_dom", SPLIT_DOM);
	conf::clnconf data(conf_dir+"data", DATA);
	vq::cpaths paths(data.val_str()+"/domains", split_dom.val_int(), split_user.val_int());

	string ql = qhome.val_str()+"/bin/qmail-local";
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
	if( chdir(paths.user_root_subpath(ext).c_str()) ) {
			cout<<"deliver: Sorry, no mailbox here by that name. (#5.1.1)"<<endl;
			return 100;
	}
	home+='/';
	home+=paths.user_root_subpath(ext);
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
