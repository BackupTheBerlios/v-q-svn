#ifndef FD_H
#define FD_H

#include <inttypes.h>

int32_t fdread(int, void *, int32_t);
int32_t fdwrite(int, const void *, int32_t);


#endif
