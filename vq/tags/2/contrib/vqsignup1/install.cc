#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>

#include "conf_conf_dir.h"
#include "conf_cgi_bin.h"
#include "conf_xslt_dir.h"
#include "conf_xslt_dir1.h"
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

int main()
{
    fdsrcdir = open(".",O_RDONLY);
    if(fdsrcdir==-1) die_errno("open: .");
            
    h(conf_conf_dir.c_str(), 0755, conf_vq_uid_i, conf_vq_gid_i);
#define d_vq(x,m) d(x,m,conf_vq_uid_i,conf_vq_gid_i)
    d_vq("conf", 0700);
#define c_vq(x) c((conf_conf_dir).c_str(),x,x,0644,conf_vq_uid_i,conf_vq_gid_i,false)
    c_vq("conf/cgi_bin");
    c_vq("conf/conf_dir");
    c_vq("conf/domains");
    c_vq("conf/html_br");
    c_vq("conf/pass_ok");
    c_vq("conf/user_ok");
    c_vq("conf/vq_user");
    c_vq("conf/xmlstart");
    c_vq("conf/xslt_dir");
    c_vq("conf/xslt_file");
    c_vq("conf/xmlstart1");
    c_vq("conf/xslt_dir1");
    c_vq("conf/xslt_file1");
    c_vq("conf/notify");
#undef c_vq
    h(conf_cgi_bin.c_str(), 0755, conf_vq_uid_i, conf_vq_gid_i);
    c(conf_cgi_bin.c_str(),"vqsignup1","vqsignup1",06555,conf_vq_uid_i,conf_vq_gid_i);
    
    h(conf_xslt_dir.c_str(), 0755, conf_vq_uid_i, conf_vq_gid_i);
    c(conf_xslt_dir.c_str(), "vqsignup1.xslt", "vqsignup1.xslt", 0644, conf_vq_uid_i, conf_vq_gid_i);
    c(conf_xslt_dir1.c_str(), "vqsignup1_notify.xslt", "vqsignup1_notify.xslt", 0644, conf_vq_uid_i, conf_vq_gid_i);
#undef d_vq
}
