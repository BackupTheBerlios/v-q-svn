#include <sys/types.h>
#include <unistd.h>

#include "fd.h"

int32_t fdread( int fd, void * b, int32_t l )
{
    int32_t i=0, r=0;
    for( ; i<l; i+=r ) {
            r = read(fd, (char*)b+i, l-i);
            switch(r) {
            case 0:
                    return i;
            case -1: 
                    return -1;
            }
    }
    return l;
}

int32_t fdwrite( int fd, const void * b, int32_t l )
{
    int32_t i=0, r=0;
    for( ; i<l; i+=r ) {
            r = write(fd, (char*)b+i, l-i);
            switch(r) {
            case 0:
                    return i;
            case -1: 
                    return -1;
            }
    }
    return l;
}
