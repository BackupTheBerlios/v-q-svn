#include <new>

#include "vq_init.h"
#include "cdaemonauth.h"
#include "cfsvq.h"

using namespace std;

/**
 *
 */
void vq_init(cvq **vq, cauth **auth, bool needauth)
{
    if( needauth ) *auth = new cdaemonauth();
    else if( auth ) *auth = NULL; 
    *vq = new cfsvq( needauth ? *auth : NULL);
}
