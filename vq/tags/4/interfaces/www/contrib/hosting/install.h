#ifndef __INSTALL_H
#define __INSTALL_H

#include <sys/types.h>

void die_errno(const char *m, int code=1);
void die_errno(const char *m, const char *m1, int code=1);
void die_errno(const char *m, const char *m1, const char *m2, int code=1);
void print_errno(const char *m, const char *m1);
void d(const char *dir, mode_t mode, int uid, int gid);
void h(const char *dir, mode_t mode, int uid, int gid );
void c(const char *src,const char *dest, mode_t mode, 
		int uid, int gid, bool over = true, bool metasave = false );
void sl(const char *src, const char *dest, bool unlnk = false);

char hier();

#endif // ifndef __INSTALL_H
