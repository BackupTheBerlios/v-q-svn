#include "install.h"

#include "conf_home.h"
#include "vquserrm_conf.h"

using namespace std;
using namespace vquserrm;

char hier() {
	h(conf_home.c_str(), 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int() );
	
	d("bin", 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int() );
    d("share", 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int() );
    d("share/vquserrm", 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int() );
    d("etc", 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int() );
    d("etc/vquserrm", 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int() );
    d("etc/vquserrm/conf", 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int() );
    
    c("vquserrm","bin/vquserrm",06555,ac_vq_uid.val_int(),ac_vq_gid.val_int());
#define c_vq(x,m) do { \
		c(x,((string)"share/vquserrm/"+x+".dist").c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true); \
		c(x,((string)"share/vquserrm/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),false); \
} while(0)
    c_vq("vquserrm.xslt", 0644);
#undef c_vq
	c("conf/vq_user", "etc/vquserrm/conf/vq_user", 0644, ac_vq_uid.val_int(), ac_vq_gid.val_int());
	c("conf/xmlstart", "etc/vquserrm/conf/xmlstart", 0644, ac_vq_uid.val_int(), ac_vq_gid.val_int());

	return 0;
}
