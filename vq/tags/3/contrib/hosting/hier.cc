#include <string>
#include <unistd.h>

#include "conf_home.h"
#include "install.h"

using namespace std;
using namespace hosting;

char hier()
{
	h(conf_home.c_str(), 0755, getuid(), getgid());
#define c_vq(x) do { \
	c(x, ((string)"share/vqwww/"+x+".dist").c_str(),0644,getuid(),getgid(),true); \
	c(x, ((string)"share/vqwww/"+x).c_str(),0644,getuid(),getgid(),false); \
} while(0)			
    c_vq("cftpmod.xslt");
    c_vq("cuseraddmod.xslt");
    c_vq("cusersmod.xslt");
    c_vq("cwwwalimod.xslt");
    c_vq("cwwwstatmod.xslt");
#undef c_vq
#define c_vq(x) c(x, ((string)"libexec/vqwww/"+x).c_str(),0644,getuid(),getgid(),true)
    c_vq("libhosting.so");
#undef c_vq
#define c_vq(x) c(x, ((string)"bin/"+x).c_str(),04755,getuid(),getgid(), true)
    c_vq("wwwalichg");
#undef c_vq
    return 0;
}
