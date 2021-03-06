#include <string>

#include "conf_home.h"
#include "vq_conf.h"

#include "install.h"

using namespace std;
using namespace vq;

char hier() {
	h(conf_home.c_str(), 0755, ac_vq_uid.val_int(), ac_vq_gid.val_int());
#define d_vq(x,m) d(x,m,ac_vq_uid.val_int(),ac_vq_gid.val_int())
	d_vq("domains",0755);
	d_vq("bin",0755);
	d_vq("sockets",01770);
	d_vq("lib",0755);
	d_vq("include",0755);
	d_vq("deleted", 01700 );
	d_vq("etc",0755);
	d_vq("etc/vq", 0755);
	d_vq("etc/vq/conf", 0700);
#undef d_vq
#define c_vq(x,m) c(x,((string)"bin/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int())
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
	c_vq("autoresp",0555);
	c_vq("pgsqlauthd",0555);
	c_vq("pgsqllogd",0555);
	c_vq("udot_ls",0555);
	c_vq("udot_add",0555);
	c_vq("udot_rm",0555);
	c_vq("qmail_assign_add", 0555);
	c_vq("qmail_assign_ex", 0555);
	c_vq("qmail_assign_rm", 0555);
	c_vq("qmail_file_add", 0555);
	c_vq("qmail_file_rm", 0555);
	c_vq("qmail_mrh_add", 0555);
	c_vq("qmail_mrh_rm", 0555);
	c_vq("qmail_rcpthosts_add", 0555);
	c_vq("qmail_run", 0555);
	c_vq("qmail_virtualdomains_add", 0555);
	c_vq("qmail_virtualdomains_rm", 0555);
	c_vq("qmail-send-restart", 0555);
#undef c_vq
#define c_vq(x,m) c(x,((string)"etc/vq/"+x).c_str(),m,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true)
	c_vq("conf/atchars", 0644);
	c_vq("conf/pgsql", 0600);
	c_vq("conf/qmail", 0644);
	c_vq("conf/qmail_mode", 0644);
	c_vq("conf/split_dom", 0644);
	c_vq("conf/split_user", 0644);
	c_vq("conf/home",0644);
	c_vq("conf/vq_mode", 0644);
	c_vq("conf/vq_user", 0644);
	c_vq("conf/autoresp_ttl", 0644);
	c_vq("conf/qmail_hyp", 0644);
	c_vq("conf/vq_fmode", 0644);
#undef c_vq
#define c_vq(x) c(x,((string)"lib/"+x).c_str(),0644,ac_vq_uid.val_int(),ac_vq_gid.val_int())
	c_vq("libsplit.so.4.0");
	c_vq("libfd.so.4.0");
	c_vq("libvq.so.4.0");
	c_vq("libfs.so.4.0");
	c_vq("libsys.so.4.0");
	c_vq("libtext.so.4.0");
	c_vq("liblog.so.4.0");
	c_vq("vq_conf.a");
#undef c_vq
	sl("libsplit.so.4.0", "lib/libsplit.so", true);
	sl("libfd.so.4.0", "lib/libfd.so", true);
	sl("libvq.so.4.0", "lib/libvq.so", true);
	sl("libfs.so.4.0", "lib/libfs.so" , true);
	sl("libsys.so.4.0", "lib/libsys.so", true);
	sl("liblog.so.4.0", "lib/liblog.so", true);
	sl("libtext.so.4.0", "lib/libtext.so", true);
#define c_vq(x) c(x,((string)"include/"+x).c_str(),0644,ac_vq_uid.val_int(),ac_vq_gid.val_int(),true)
	c_vq("split.h");
	c_vq("fd.h");
	c_vq("fdstr.h");
	c_vq("getline.h");
	c_vq("cvq.h");
	c_vq("cauth.h");
	c_vq("dirs.h");
	c_vq("lock.h");
	c_vq("util.h");
	c_vq("sig.h");
	c_vq("lower.h");
	c_vq("sys.h");
	c_vq("vq_conf.h");
	c_vq("cconf.h");
	c_vq("clogger.h");
#undef c_vq
	return 0;
}
