#include <unistd.h>

#include "install.h"
#include "conf_home.h"

using namespace std;
using namespace valid;

char hier() {
    int uid = getuid(), gid=getgid();

    h(conf_home.c_str(), 0755, uid, gid);

#define d_vq(x,m) d(x,m,uid,gid)
    d_vq("lib", 0755);
    d_vq("include", 0755);

    c("libvalid.so.4.0","lib/libvalid.so.4.0",0644,uid,gid);
    sl("libvalid.so.4.0", "lib/libvalid.so", true);
    c("valid.h", "include/valid.h", 0644, uid, gid);
	return 0;
}
