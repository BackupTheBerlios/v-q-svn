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
#include "main.h"
#include "install.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <iostream>
#include <cerrno>
#include <string>
#include <fstream>

using namespace std;

int fdsrcdir=-1, fddestdir=-1;

/**
 * Print error message, system error message and throw exit code.
 */
void die_errno(const char *m, int code) {
	cerr<<m<<": "<<strerror(errno)<<endl;
	throw code;
}

void die_errno(const char *m, const char *m1, int code) {
	cerr<<m<<": "<<m1<<": "<<strerror(errno)<<endl;
	throw code;
}

void die_errno(const char *m, const char *m1, const char *m2, int code) {
	cerr<<m<<": "<<m1<<": "<<m2<<": "<<strerror(errno)<<endl;
	throw code;
}

/**
 * 
 */
void print_errno(const char *m, const char *m1) {
	cerr<<m<<": "<<m1<<": "<<strerror(errno)<<endl;
}

/**
 * Create directory, change owner and mode. If exists modes and owner are
 * not changed.
 */
void d(const char *dir, mode_t mode, int uid, int gid)
{
	if(mkdir(dir, mode)) {
			if(errno != EEXIST) die_errno("mkdir", dir);
			else print_errno("mkdir", dir);
			return;
	}
	if( chown(dir, uid, gid) == -1 ) die_errno("chown",dir);
	if( chmod(dir, mode) == -1 ) die_errno("chmod", dir);
}

/**
 * Create directory and chdir to it.
 */
void h(const char *dir, mode_t mode, int uid, int gid ) {
	d(dir,mode,uid,gid);
	if( chdir(dir) ) die_errno("chdir",dir);
	if( fddestdir != -1 ) close(fddestdir);
	if( (fddestdir = open(".", O_RDONLY)) == -1 ) die_errno("open", ".");
}

/**
 * Copy file.
 * \param src source file in fdsrcdir
 * \param dest destination file in fddestdir
 * \param mode file's mode
 * \param uid 
 * \param gid
 * \param over overwrite
 * \param metasave when overwritting save current owner and mode
 */
void c(const char *src,const char *dest, mode_t mode, 
		int uid, int gid, bool over, bool metasave ) {
	struct stat st;
	if(fchdir(fdsrcdir)) die_errno("chdir: srcdir");

	ifstream in(src);
	if(!in) die_errno("open",src);

	if(fchdir(fddestdir)) die_errno("chdir: destdir");
	if( ! stat(dest, &st) ) {
			if(! over) {
					cerr<<dest<< ": exists, won't overwrite"<<endl;
					return;
			}
			if(metasave) {
					uid = st.st_uid;
					gid = st.st_gid;
					mode = st.st_mode;
			}
	}

	ofstream out(dest,ios::trunc);
	if(!out) die_errno("open",dest);

	out<<in.rdbuf();
	out.close();
	if(out.bad()) die_errno("write",dest);
	if(in.bad()) die_errno("read",src);
	if( chown(dest,uid,gid)) die_errno("chown",dest);
	if( chmod(dest,mode) ) die_errno("chmod",dest);
}

/**
 * Do a symlink.
 * \param unlnk if true destination is unlinked first
 */
void sl(const char *src, const char *dest, bool unlnk ) {
	if(unlnk) {
			if(unlink(dest) && errno!=ENOENT) die_errno("unlink", dest); 
	}
	if(symlink(src, dest)) {
			die_errno("symlink", src, dest);
	}
}

/**
 * 
 */
int cppmain( int, char ** ) {
	try {
			fdsrcdir = open(".",O_RDONLY);
			if(fdsrcdir==-1) die_errno("open: .");
			if(hier()) return 111;
	} catch( int ec ) {
			cerr<<"dieing: exit code: "<<ec<<endl;
			return ec;
	} catch(...) {
			cerr<<"dieing: unknown exception"<<endl;
			return 111;
	}
	return 0;
}
