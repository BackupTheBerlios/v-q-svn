#include <string>

#include "conf_home.h"
#include "vqwww_conf.h"

#include "install.h"

using namespace std;

char hier() {
    h(conf_home.c_str(), 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int());
#define d_vq(x,m) d(x,m,ac_vq_uid.val_int(),ac_vq_gid.val_int())
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
    d_vq("etc/vqwww/conf", 0755);
#undef d_vq
#define c_vq(x,m) c(x,((string)"bin/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true,true)
    c_vq("vqwww",06555);
#undef c_vq
#define c_vq(x,m) c(((string)"conf/"+x).c_str(),((string)"etc/vqwww/conf/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true)
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
    c_vq("vq_user", 0644);
#undef c_vq
#define c_vq(x,m) do { \
		c(x,((string)"share/vqwww/"+x+".dist").c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true); \
		c(x,((string)"share/vqwww/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),false); \
} while(0)
		
    c_vq("vqwww.xslt", 0644);
    c_vq("credirmod.xslt", 0644);
    c_vq("cpassmod.xslt", 0644);
    c_vq("cautorespmod.xslt", 0644);
#undef c_vq
#define c_vq(x) c(x,((string)"include/vqwww/"+x).c_str(),0644,ac_vq_uid.val_int(),ac_vq_gid.val_int())
    c_vq("vqwww.h");
    c_vq("cmod.h");
    c_vq("cgi.h");
    c_vq("html.h");
    c_vq("csess.h");
	c_vq("vqwww_conf.h");
	c_vq("cconf.h");
#undef c_vq
#define c_vq(x) c(x,((string)"lib/vqwww/"+x).c_str(),0644,ac_vq_uid.val_int(),ac_vq_gid.val_int())
    c_vq("libsess.so.1");
    c_vq("vqwww_conf.a");
    sl("libsess.so.1", "lib/vqwww/libsess.so", true);
#undef c_vq
    return 0;
} 
