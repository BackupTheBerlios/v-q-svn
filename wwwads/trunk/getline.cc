#include <unistd.h>

#include "getline.h"

using namespace std;

/**
read line from input
*/
bool getline( int fd, string & b, char delim )
{
    b = "";
    char c;
    for(;;) {
            switch(read(fd,&c,1)) {
            case 1:
                if(c==delim)
                        return true;
                b+=c;
                break;
            case 0: 
            case -1:
                return false;
            }
    }
}
