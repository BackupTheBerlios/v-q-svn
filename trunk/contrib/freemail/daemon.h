#ifndef __DAEMON_H
#define __DAEMON_H

#include <stdexcept>

#include "sys.h"

extern int sso, cso;
extern cpoll fds;

void setup();
char child(int);

/**
 * This exception is thrown when can't read from child.
 */
class rd_error : public std::runtime_error {
		public:
				rd_error();
};

/**
 * This exception is thrown when can't write to child.
 */
class wr_error : public std::runtime_error {
		public:
				wr_error();
};

#endif
