#include <sys/types.h>
#include <md5.h>

int main()
{
    unsigned char digest[16];
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx,digest,6);
    MD5Final(digest,&ctx);
    return 0;
}
