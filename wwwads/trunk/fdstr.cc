#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

#include "fdstr.h"
#include "fd.h"

int fdrdstr(int fd, string & str) throw()
{
    uint32_t len, i;
    char c;
    str = "";
    if( fdread(fd, &len, sizeof(len)) == sizeof(len) ) {
            str.reserve(len);
            for( i=0; i<len; i++ ) {
                    if( read(fd, &c, 1) != 1 )
                            return -1;
                    str += c;
            }
            return len;
    }
    return -1;
}

/**
write string as: uint32_t len, char[len] data
@return -1 on error, len on success
*/
int fdwrstr(int fd, const string & str) throw()
{
    uint32_t len = str.length();
    if( fdwrite(fd, &len, sizeof(len)) == sizeof(len)
        && fdwrite(fd, str.data(), len) == len )
            return len;
    return -1;
}
