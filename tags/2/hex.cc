#include "sys.h"

using namespace std;

/*
 */
string to_hex(const unsigned char *buf, unsigned bufl)
{
    static const unsigned char hex[] = "0123456789abcdef";
    string hexbuf;
    hexbuf.reserve(bufl<<1);
    for( unsigned i=0; i < bufl; i++ ) {
            hexbuf += hex[ (buf[i] & 0xf0) >> 4 ];
            hexbuf += hex[ buf[i] & 0xf ]; 
    }
    return hexbuf;
}
