#include <string>

#include "conf_home.h"
#include "freemail_conf.h"

#include "install.h"

using namespace std;
using namespace freemail;

char hier() {
    h(conf_home.c_str(), 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int());
#define d_vq(x,m) d(x,m,ac_vq_uid.val_int(),ac_vq_gid.val_int())
    d_vq("bin", 0755 );
    d_vq("share", 0755 );
    d_vq("share/freemail", 0755 );
    d_vq("var", 0755 );
    d_vq("var/run", 0755 );
    d_vq("var/run/vqsignup1d", 0700 );
    d_vq("libexec", 0755 );
    d_vq("libexec/freemail", 0755 );
    d_vq("etc", 0755 );
    d_vq("etc/freemail", 0755 );
    d_vq("etc/freemail/conf", 0755 );
#undef d_vq 
#define c_vq(x,m) c(x,((string)"bin/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true,true)
    c_vq("vqsignup1",06555);
    c_vq("vqrescue",06555);
    c_vq("vqsignup1d",0555);
#undef c_vq
#define c_vq(x,m) c(x,((string)"libexec/freemail/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true,true)
    c_vq("libfreemail.so", 0644);
#undef c_vq
#define c_vq(x,m) c(x,((string)"share/freemail/"+x+".dist").c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true)
    c_vq("vqsignup1.xslt", 0644);
    c_vq("freemail.xslt", 0644);
#undef c_vq	
#define c_vq(x,m) c(x,((string)"share/freemail/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),false)
    c_vq("vqsignup1.xslt", 0644);
    c_vq("freemail.xslt", 0644);
	return 0;
}
