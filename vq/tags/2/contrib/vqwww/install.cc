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
            
    h(conf_home.c_str(), 0755, conf_vq_uid_i, conf_vq_gid_i);
#define d_vq(x,m) d(x,m,conf_vq_uid_i,conf_vq_gid_i)
    d_vq("bin",0755);
    d_vq("lib",0755);
    d_vq("lib/vqwww", 0755);
    d_vq("libexec",0755);
    d_vq("libexec/vqwww", 0755);
    d_vq("share", 0755);
    d_vq("share/vqwww", 0755);
    d_vq("include", 0755);
    d_vq("include/vqwww", 0755);
    d_vq("etc/", 0755);
    d_vq("etc/vqwww", 0755);
#undef d_vq
#define c_vq(x,m) c((conf_home+"/bin").c_str(),x,x,m,conf_vq_uid_i,conf_vq_gid_i)
    c_vq("vqwww",06555);
#undef c_vq
#define c_vq(x,m) c((conf_home+"/share/vqwww").c_str(),((string)"conf/"+x).c_str(),x,m,conf_vq_uid_i,conf_vq_gid_i,true)
    c_vq("fs", 0644);
    c_vq("fs_split",0644);
    c_vq("html_br", 0644);
    c_vq("pass_maxl", 0644);
    c_vq("pass_minl", 0644);
    c_vq("pass_ok", 0644);
    c_vq("redir_max", 0644);
    c_vq("sess_timeout", 0644);
    c_vq("user_maxl",0644);
    c_vq("user_minl", 0644);
    c_vq("user_ok", 0644);
    c_vq("xmlstart", 0644);
    c_vq("xslt_file", 0644);
    c_vq("vq_user", 0644);
#undef c_vq
#define c_vq(x,m) c((conf_home+"/share/vqwww").c_str(),x,x,m,conf_vq_uid_i,conf_vq_gid_i,false)
    c_vq("vqwww.xslt", 0644);
    c_vq("credirmod.xslt", 0644);
    c_vq("cpassmod.xslt", 0644);
#undef c_vq
#define c_vq(x) c((conf_home+"/include/vqwww").c_str(),x,x,0644,conf_vq_uid_i,conf_vq_gid_i)
    c_vq("vqwww.h");
    c_vq("cmod.h");
    c_vq("cgi.h");
    c_vq("html.h");
    c_vq("conf_pass_maxl.h");
    c_vq("conf_pass_minl.h");
    c_vq("conf_pass_ok.h");
    c_vq("conf_user_ok.h");
    c_vq("conf_user_maxl.h");
    c_vq("conf_user_minl.h");
    c_vq("csess.h");
#undef c_vq
#define c_vq(x) c((conf_home+"/lib/vqwww").c_str(),x,x,0644,conf_vq_uid_i,conf_vq_gid_i)
    c_vq("libsess.so.1");
    sl((conf_home+"/lib/vqwww").c_str(), "libsess.so.1", "libsess.so");
#undef c_vq
    return 0;
}
