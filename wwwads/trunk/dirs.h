#ifndef __DIRS_H
#define __DIRS_H

#include <sys/types.h>
#include <stdexcept>
#include <deque>
#include <string>

using namespace std;

bool mkdirhier( const char *, mode_t );
bool rmdirrec( const string & n ) throw(runtime_error);
bool rmdirsrec( const deque<string> &, bool = true ) throw(runtime_error);

#endif
