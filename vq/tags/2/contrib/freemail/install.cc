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

void p_errno2(const char *m, const char *m1) {
    cerr<<m<<": "<<m1<<": "<<strerror(errno)<<endl;
}

void p_errno3(const char *m, const char *m1, const char *m2) {
    cerr<<m<<": "<<m1<<'/'<<m2<<": "<<strerror(errno)<<endl;
}

void die_errno2(const char *m, const char *m1) 
{
    p_errno2(m, m1);
    exit(1);
}

void d(const char *dir, mode_t mode, int uid = getuid(), int gid = getgid())
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

void d(const char *dir, const char *sub, mode_t mode, int uid = getuid(), int gid = getgid())
{
    if(chdir(dir)) die_errno2("chdir", dir);
    if(mkdir(sub, mode)) {
            if(errno != EEXIST)
                    die_errno2("mkdir", sub);
            else p_errno2("mkdir", sub);
            return;
    }
    if( chown(sub, uid, gid) == -1 )
            die_errno2("chown",sub);
    if( chmod(sub, mode) == -1 )
            die_errno2("chmod",sub);
}

void h(const char *dir, mode_t mode, int uid = getuid(), int gid = getgid())
{
    d(dir,mode,uid,gid);
    if( chdir(dir) )
            die_errno2("chdir",dir);
}

void h(const char *dir, const char *sub, mode_t mode, int uid = getuid(), int gid = getgid())
{
    d(dir,sub,mode,uid,gid);
    if( chdir(dir) ) die_errno2("chdir",dir);
    if( chdir(sub) ) die_errno2("chdir", sub);
}

void c(const char * home,const char *sub,const char *src,const char *dest, mode_t mode, 
        int uid=getuid(), int gid=getgid(), bool over = true )
{
    struct stat st;
    if(fchdir(fdsrcdir)) die_errno("chdir: srcdir");
    ifstream in(src);
    if(!in) { p_errno2("open",src); return; }
    if(chdir(home)) { p_errno2("chdir",home); return; }
    if(chdir(sub)) { p_errno2("chdir",sub); return; }
    if( ! stat(dest, &st) ) {
            if(! over) {
                    cerr<<home<<'/'<<dest<<": exists, won't overwrite"<<endl;
                    return;
            }
            uid = st.st_uid;
            gid = st.st_gid;
            mode = st.st_mode;
    }
    ofstream out(dest,ios::trunc,mode);
    if(!out) { p_errno3("open",home,((string)sub+'/'+dest).c_str()); return; }
    out<<in.rdbuf();
    out.close();
    if(!out) { p_errno2("write",((string)home+'/'+dest).c_str()); return; }
    if(in.bad()) { p_errno2("read",src); return; }
    if( chown(dest,uid,gid)) { p_errno2("chown",((string)home+'/'+dest).c_str()); return; }
    if( chmod(dest,mode) ) { p_errno2("chmod",((string)home+'/'+dest).c_str()); return; }
}

int main()
{
    const char *conf_home = ::conf_home.c_str();
    fdsrcdir = open(".",O_RDONLY);
    if(fdsrcdir==-1) die_errno("open: .");

    d(conf_home, 0755, conf_vq_uid_i, conf_vq_gid_i );
    d(conf_home,"share", 0755, conf_vq_uid_i, conf_vq_gid_i );
    d(conf_home,"var", 0755, conf_vq_uid_i, conf_vq_gid_i );
    d(conf_home,"var/run", 0755, conf_vq_uid_i, conf_vq_gid_i );
    d(conf_home,"var/run/vqsignup1d", 0700, conf_vq_uid_i, conf_vq_gid_i );
    d(conf_home,"libexec", 0755, conf_vq_uid_i, conf_vq_gid_i);
    
    h(conf_home,"bin", 0755, conf_vq_uid_i, conf_vq_gid_i);
    c(conf_home,"bin","vqsignup1","vqsignup1",06555,conf_vq_uid_i,conf_vq_gid_i);
    c(conf_home,"bin","vqrescue","vqrescue",06555,conf_vq_uid_i,conf_vq_gid_i);
    c(conf_home,"bin","vqsignup1d","vqsignup1d",0555,conf_vq_uid_i,conf_vq_gid_i);
    
    h(conf_home,"libexec/freemail", 0755, conf_vq_uid_i, conf_vq_gid_i);
    c(conf_home,"libexec/freemail", "libfreemail.so", "libfreemail.so", 0644, conf_vq_uid_i, conf_vq_gid_i);
            
    h(conf_home,"share/freemail", 0755, conf_vq_uid_i, conf_vq_gid_i);
    c(conf_home,"share/freemail", "vqsignup1.xslt", "vqsignup1.xslt.dist", 0644, conf_vq_uid_i, conf_vq_gid_i);
    c(conf_home,"share/freemail", "freemail.xslt", "vqsignup1.xslt.dist", 0644, conf_vq_uid_i, conf_vq_gid_i);
}
