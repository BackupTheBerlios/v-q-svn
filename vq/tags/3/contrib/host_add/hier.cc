#include "install.h"
#include "conf_home.h"
#include "host_add_conf.h"

using namespace std;
using namespace host_add;

char hier() {
    h(conf_home.c_str(), 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int()); 
#define d_vq(x,m) d(x,m,ac_vq_uid.val_int(),ac_vq_gid.val_int())
	d_vq("bin", 0755);
	d_vq("share", 0755);
	d_vq("share/host_add", 0755);
	d_vq("etc", 0755);
	d_vq("etc/host_add", 0755);
#undef d_vq
#define c_vq(x,y,m) c(x,y,m,ac_vq_uid.val_int(),ac_vq_gid.val_int())
    c_vq("host_add","bin/host_add", 06555);
    c_vq("host_add.xslt","share/host_add/host_add.xslt.dist", 0644);
#undef c_vq
	return 0;
}
