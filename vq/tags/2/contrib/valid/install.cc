#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>

#include "conf_home.h"

using namespace std;

int fdsrcdir=-1;

void die_errno(const char *m)
{
    cerr<<m<<": "<<strerror(errno)<<endl;
    exit(1);
}

void p_errno2(const char *m, const char *m1) {
    cerr<<m<<": "<<m1<<": "<<strerror(errno)<<endl;
}

void die_errno2(const char *m, const char *m1) 
{
    p_errno2(m, m1);
    exit(1);
}

void d(const char *dir, mode_t mode, int uid, int gid)
{
    if(mkdir(dir, mode)) {
            if(errno != EEXIST)
                    die_errno2("mkdir", dir);
            else p_errno2("mkdir", dir);
            return;
    }
    if( chown(dir, uid, gid) == -1 )
            die_errno2("chown",dir);
    if( chmod(dir, mode) == -1 )
            die_errno2("chmod", dir);
}
void h(const char *dir, mode_t mode, int uid, int gid )
{
    d(dir,mode,uid,gid);
    if( chdir(dir) )
            die_errno2("chdir",dir);
}
void c(const char * home,const char *src,const char *dest, mode_t mode, int uid, int gid, bool over = true )
{
    struct stat st;
    if(fchdir(fdsrcdir)) die_errno("chdir: srcdir");
    ifstream in(src);
    if(!in) { p_errno2("open",src); return; }
    if(chdir(home)) { p_errno2("chdir",home); return; }
    if( ! stat(dest, &st) ) {
            if(! over) {
                    cerr<<home<<'/'<<dest<<": exists, won't overwrite"<<endl;
                    return;
            }
            uid = st.st_uid;
            gid = st.st_gid;
            mode = st.st_mode;
    }
    ofstream out(dest,ios::trunc);
    if(!out) { p_errno2("open",((string)home+'/'+dest).c_str()); return; }
    out<<in.rdbuf();
    out.close();
    if(!out) { p_errno2("write",((string)home+'/'+dest).c_str()); return; }
    if(in.bad()) { p_errno2("read",src); return; }
    if( chown(dest,uid,gid)) { p_errno2("chown",((string)home+'/'+dest).c_str()); return; }
    if( chmod(dest,mode) ) { p_errno2("chmod",((string)home+'/'+dest).c_str()); return; }
}
void sl(const char *home, const char *src, const char *dest,bool unl=false) {
    if(chdir(home)) { p_errno2("chdir",home); return; }
    if(unl && unlink(dest)) {
      cerr<<"unlink: "<<dest<<endl;
    }
    if(symlink(src, dest)) {
            cerr<<"symlink: "<<src<<" to: "<< dest <<": "<<strerror(errno)<<endl;
    }
}
int main()
{
    fdsrcdir = open(".",O_RDONLY);
    if(fdsrcdir==-1) die_errno("open: .");
           
    int uid = getuid(), gid=getgid();
    h(conf_home.c_str(), 0755, uid, gid);
#define d_vq(x,m) d(x,m,uid,gid)
    d_vq("lib", 0755);
    d_vq("include", 0755);

    c((conf_home+"/lib").c_str(),"libvalid.so.1","libvalid.so.1",0644,uid,gid);
    sl((conf_home+"/lib").c_str(), "libvalid.so.1", "libvalid.so", true);
    c((conf_home+"/include").c_str(), "valid.h", "valid.h", 0644, uid, gid);
}
