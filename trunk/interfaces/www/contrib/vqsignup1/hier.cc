#include "install.h"
#include "conf_home.h"
#include "vqsignup1_conf.h"

using namespace std;
using namespace vqsignup1;

char hier() {
    h(conf_home.c_str(), 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int());
#define d_vq(x,m) d(x,m,ac_vq_uid.val_int(),ac_vq_gid.val_int())
	d_vq("bin", 0755);
	d_vq("share", 0755);
	d_vq("share/vqsignup1", 0755);
	d_vq("etc", 0755);
	d_vq("etc/vqsignup1", 0755);
    d_vq("etc/vqsignup1/conf", 0755);
#define c_vq(x) c(((string)"conf/"+x).c_str(),((string)"etc/vqsignup1/conf/"+x).c_str(),0644,ac_vq_uid.val_int(),ac_vq_gid.val_int())
    c_vq("domains");
    c_vq("html_br");
    c_vq("pass_ok");
    c_vq("user_ok");
    c_vq("vq_user");
    c_vq("xmlstart");
    c_vq("xmlstart1");
    c_vq("notify");
#undef c_vq
	
    c("vqsignup1","bin/vqsignup1",06555,ac_vq_uid.val_int(),ac_vq_gid.val_int());
    c("vqsignup1.xslt", "share/vqsignup1/vqsignup1.xslt.dist", 0644, ac_vq_uid.val_int(), ac_vq_gid.val_int());
    c("vqsignup1_notify.xslt", "share/vqsignup1/vqsignup1_notify.xslt.dist", 0644, ac_vq_uid.val_int(), ac_vq_gid.val_int());
	return 0;
}
