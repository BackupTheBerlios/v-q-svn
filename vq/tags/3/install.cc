#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>

using namespace std;

int fdsrcdir=-1, fddestdir=-1;

// user's defined function
int hier();

/**
 * Print error message, system error message and throw exit code.
 */
void die_errno(const char *m, int code=1) {
	cerr<<m<<": "<<strerror(errno)<<endl;
	throw code;
}

void die_errno(const char *m, const char *m1, int code=1) 
{
	cerr<<m<<": "<<m1<<": "<<strerror(errno)<<endl;
	throw code;
}

void die_errno(const char *m, const char *m1, const char *m2, int code=1) 
{
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
		int uid, int gid, bool over = true, bool metasave = false ) {
	struct stat st;
	if(fchdir(fdsrcdir)) die_errno("chdir: srcdir");

	ifstream in(src);
	if(!in) die_errno("open",src);

	if(fchdir(fddestdir)) die_errno("chdir: destdir");
	if( ! stat(dest, &st) ) {
			if(! over) die_errno(dest, "exists, won't overwrite");
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
	if(!out) die_errno("write",dest);
	if(in.bad()) die_errno("read",src);
	if( chown(dest,uid,gid)) die_errno("chown",dest);
	if( chmod(dest,mode) ) die_errno("chmod",dest);
}

/**
 * Do a symlink.
 * \param unlnk if true destination is unlinked first
 */
void sl(const char *src, const char *dest, bool unlnk = false) {
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
int main() {
	try {
			fdsrcdir = open(".",O_RDONLY);
			if(fdsrcdir==-1) die_errno("open: .");
			if(hier()) return 111;
	} catch( int ec ) {
			return ec;
	}
	return 0;
}
