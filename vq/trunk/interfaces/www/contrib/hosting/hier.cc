#include <string>
#include <unistd.h>

#include "conf_home.h"
#include "hosting_conf.h"
#include "install.h"

using namespace std;
using namespace hosting;

char hier()
{
	h(conf_home.c_str(), 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int());
#define d_vq(x,m) d(x,m,ac_vq_uid.val_int(),ac_vq_gid.val_int())
    d_vq("etc",0755);
    d_vq("etc/hosting",0755);
    d_vq("etc/hosting/conf",0755);

#define c_vq(x,m) c(((string)"conf/"+x).c_str(),((string)"etc/hosting/conf/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true)
	c_vq("ftp_base", 0644);
	c_vq("html_br", 0644);
	c_vq("pass_minl", 0644);
	c_vq("prefix_ok", 0644);
	c_vq("user_ok", 0644);
	c_vq("ftp_ok", 0644);
	c_vq("vq_user", 0644);
	c_vq("notify", 0644);
	c_vq("pass_ok", 0644);
	c_vq("user_maxl", 0644);
	c_vq("wwwali_exe", 0644);
	c_vq("home", 0644);
	c_vq("pass_maxl", 0644);
	c_vq("pgsql", 0600);
	c_vq("user_minl", 0644);
#undef c_vq	
#define c_vq(x) do { \
	c(x, ((string)"share/vqwww/"+x+".dist").c_str(),0644,ac_vq_uid.val_int(), ac_vq_gid.val_int(),true); \
	c(x, ((string)"share/vqwww/"+x).c_str(),0644,ac_vq_uid.val_int(), ac_vq_gid.val_int(),false); \
} while(0)			
    c_vq("cftpmod.xslt");
    c_vq("cuseraddmod.xslt");
    c_vq("cusersmod.xslt");
    c_vq("cwwwalimod.xslt");
    c_vq("cwwwstatmod.xslt");
#undef c_vq
#define c_vq(x) c(x, ((string)"libexec/vqwww/"+x).c_str(),0644,ac_vq_uid.val_int(), ac_vq_gid.val_int(),true)
    c_vq("libhosting.so");
#undef c_vq
#define c_vq(x) c(x, ((string)"bin/"+x).c_str(),04755,getuid(),getgid(), true)
    c_vq("wwwalichg");
#undef c_vq
    return 0;
}
