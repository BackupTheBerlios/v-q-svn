#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>

#include "conf_vq.h"
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
    ofstream out(dest,ios::trunc,mode);
    if(!out) { p_errno2("open",((string)home+'/'+dest).c_str()); return; }
    out<<in.rdbuf();
    out.close();
    if(!out) { p_errno2("write",((string)home+'/'+dest).c_str()); return; }
    if(in.bad()) { p_errno2("read",src); return; }
    if( chown(dest,uid,gid)) { p_errno2("chown",((string)home+'/'+dest).c_str()); return; }
    if( chmod(dest,mode) ) { p_errno2("chmod",((string)home+'/'+dest).c_str()); return; }
}
void sl(const char *home, const char *src, const char *dest) {
    if(chdir(home)) { p_errno2("chdir",home); return; }
    if(symlink(src, dest)) {
            cerr<<"symlink: "<<src<<" to: "<< dest <<": "<<strerror(errno)<<endl;
    }
}
int main()
{
    fdsrcdir = open(".",O_RDONLY);
    if(fdsrcdir==-1) die_errno("open: .");
            
    h(conf_vq.c_str(), 0755, conf_vq_uid_i, conf_vq_gid_i);
#define d_vq(x,m) d(x,m,conf_vq_uid_i,conf_vq_gid_i)
    d_vq("domains",0755);
    d_vq("bin",0755);
    d_vq("sockets",01770);
    d_vq("lib",0755);
    d_vq("include",0755);
    d_vq("conf", 0700);
    d_vq("deleted", 01700 );
#undef d_vq
#define c_vq(x,m) c((conf_vq+"/bin").c_str(),x,x,m,conf_vq_uid_i,conf_vq_gid_i)
    c_vq("dom_add",0555);
    c_vq("user_add",0555);
    c_vq("dom_alias_add",0555);
    c_vq("user_pass",0555);
    c_vq("user_rm",0555);
    c_vq("dom_rm",0555);
    c_vq("dom_alias_rm",0555);
    c_vq("user_auth",0555);
    c_vq("dom_ip_add",0555);
    c_vq("dom_ip_rm",0555);
    c_vq("dom_ip_ls",0555);
    c_vq("qt_over",0555);
    c_vq("qt_set",0555);
    c_vq("qt_get",0555);
    c_vq("qt_cur_set",0555);
    c_vq("qt_def_set",0555);
    c_vq("qt_def_get",0555);
    c_vq("deliver",0555);
    c_vq("pgsqlauthd",0555);
    c_vq("pgsqllogd",0555);
#undef c_vq
#define c_vq(x,m) c((conf_vq).c_str(),x,x,m,conf_vq_uid_i,conf_vq_gid_i,true)
    c_vq("conf/atchars", 0644);
    c_vq("conf/log",0644);
    c_vq("conf/pgsql", 0600);
    c_vq("conf/ps", 0644);
    c_vq("conf/qmail", 0644);
    c_vq("conf/qmail_mode", 0644);
    c_vq("conf/split_dom", 0644);
    c_vq("conf/split_user", 0644);
    c_vq("conf/vq",0644);
    c_vq("conf/vq_mode", 0644);
    c_vq("conf/vq_user", 0644);
#undef c_vq
#define c_vq(x) c((conf_vq).c_str(),x,((string)"lib/"+x).c_str(),0644,conf_vq_uid_i,conf_vq_gid_i)
    c_vq("libsplit.so.1");
    c_vq("libfd.so.1");
    c_vq("libvq.so.1");
    c_vq("libfs.so.1");
    c_vq("libsys.so.1");
    c_vq("libtext.so.1");
#define sl_vq(x, y) sl((conf_vq+"/lib").c_str(), x, y)
    sl_vq("libsplit.so.1", "libsplit.so");
    sl_vq("libfd.so.1", "libfd.so");
    sl_vq("libvq.so.1", "libvq.so");
    sl_vq("libfs.so.1", "libfs.so" );
    sl_vq("libsys.so.1", "libsys.so");
    sl_vq("libtext.so.1", "libtext.so");
#undef sl_vq
#undef c_vq
#define c_vq(x) c((conf_vq).c_str(),x,((string)"include/"+x).c_str(),0644,conf_vq_uid_i,conf_vq_gid_i)
    c_vq("split.h");
    c_vq("fd.h");
    c_vq("fdstr.h");
    c_vq("getline.h");
    c_vq("cvq.h");
    c_vq("cauth.h");
    c_vq("vq_init.h");
    c_vq("dirs.h");
    c_vq("lock.h");
    c_vq("util.h");
    c_vq("skillall.h");
    c_vq("sig.h");
    c_vq("lower.h");
    c_vq("sys.h");
#undef c_vq
}
