#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>

#include "conf_home.h"
#include "conf_vq_uid.h"
#include "conf_vq_gid.h"

using namespace std;

int fdsrcdir=-1;

void die_errno(const char *m)
{
    cerr<<m<<": "<<strerror(errno)<<endl;
    exit(1);
}

void die_errno2(const char *m, const char *m1) 
{
    cerr<<m<<": "<<m1<<": "<<strerror(errno)<<endl;
    exit(1);
}

void d(const char *dir, mode_t mode, int uid, int gid)
{
    if(mkdir(dir, mode) && errno!=EEXIST)
            die_errno2("mkdir", dir);
    if( chmod(dir, mode) == -1 )
            die_errno2("chmod", dir);
    if( chown(dir, uid, gid) == -1 )
            die_errno2("chown",dir);
}
void c(const char * home,const char *src,const char *dest, mode_t mode, int uid, int gid )
{
    if(fchdir(fdsrcdir)) die_errno("chdir: srcdir");
    ifstream in(src);
    if(!in) die_errno2("open",src);
    if(chdir(home)) die_errno2("chdir",home);
    ofstream out(dest,ios::trunc,mode);
    if(!out) die_errno2("open",((string)home+'/'+dest).c_str());
    out<<in.rdbuf();
    out.close();
    if(!out) die_errno2("write",((string)home+'/'+dest).c_str());
    if(in.bad()) die_errno2("read",src);
    if( chown(dest,uid,gid)) die_errno2("chown",((string)home+'/'+dest).c_str());
    if( chmod(dest, mode) ) die_errno2("chmod",((string)home+'/'+dest).c_str());
}

int main()
{
    fdsrcdir = open(".",O_RDONLY);
    if(fdsrcdir==-1) die_errno("open: .");
            
    if( chdir(conf_home.c_str()) )
            die_errno2("chdir",conf_home.c_str());
#define d_vq(x,m) d(x,m,conf_vq_uid_i,conf_vq_gid_i)
//    d_vq("conf",0755);
#undef d_vq
#define c_vq(x,m) c(conf_home.c_str(),x,x,m,conf_vq_uid_i,conf_vq_gid_i)
    c_vq("host_add",06555);
    c_vq("host_add.xslt", 0644);
    c_vq("host_add.dom", 0644);
#undef c_vq
#define c_vq(x,m) c(conf_home.c_str(),x,x,m,conf_vq_uid_i,conf_vq_gid_i)
//    c_vq("conf/vq_user", 0644);
#undef c_vq
}
