#ifndef __SYS_H
#define __SYS_H

#include <sys/poll.h>
#include <inttypes.h>
#include <stdexcept>

#include "sig.h"
#include "skillall.h"

/// returns representation of gettimeofday in TAI64 format, it may
/// return invalid date on gettimeofday error
const char *tai64_now() throw();

// return data in hex representation
std::string to_hex(const unsigned char *, unsigned );
std::string hex_from(const std::string & );

/**
 * simple wrapper for poll and struct pollfd, allows you easily add, remove
 * struct pollfd, and poll them
 */
class cpoll {
protected:
		uint32_t left;
public:
		struct pollfd *fds;
		uint32_t cnt;

		cpoll() throw (std::bad_alloc);
		~cpoll();
	
		void add(int , short) throw(std::bad_alloc);
		void rm(int) throw();
		void chg(int, short) throw(); 
		int poll(int) throw();
		const struct pollfd & operator [] (int) const throw() ;
};

#endif
