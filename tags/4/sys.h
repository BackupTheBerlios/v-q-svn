#ifndef __SYS_H
#define __SYS_H

#include <sys/poll.h>
#include <inttypes.h>
#include <stdexcept>

#include "sig.h"

/// returns representation of gettimeofday in TAI64 format, it may
/// return invalid date on gettimeofday error
const char *tai64_now();

// return data in hex representation
std::string to_hex(const unsigned char *, unsigned );
std::string hex_from(const std::string & );

/**
 * simple wrapper for poll and struct pollfd, allows you easily add, remove
 * struct pollfd, and poll them
 */
class cpoll {
protected:
		uint32_t left; //!< number of structures that are allocated but unused
public:
		struct pollfd *fds; //!< descriptors poll
		uint32_t cnt; //!< number of descriptions in fds

		cpoll();
		~cpoll();
	
		void add(int , short); //!< add desc.
		void rm(int); //!< remove desc.
		void chg(int, short); //!< change poll options for desc.
		int poll(int); //!< call poll on fds
		const struct pollfd & operator [] (int) const; //!< easy access
};

#endif
