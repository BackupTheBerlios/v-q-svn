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
