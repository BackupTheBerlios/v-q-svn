#include "sys.h"

using namespace std;
/**
 *
 */
cpoll::cpoll() 
		: fds(NULL), cnt(0), left(0) {
	fds = new struct pollfd [1];
	cnt = left = 1;
	memset(fds, 0, sizeof(*fds));
}

/**
 *
 */
cpoll::~cpoll() {
	delete [] fds;
}

/**
 *
 */
void cpoll::add(int fd, short ev) {
	uint32_t i;
	for( i=0; i < cnt; i++ )
			if( fds[i].fd == fd ) {
					fds[i].events = ev;
					fds[i].revents = 0;
					return;
			}
	if(!left) {
			struct pollfd *fds_tmp = new struct pollfd [cnt<<1];
			for( i=0; i < cnt; i++ )
					fds_tmp[i] = fds[i];
			left = cnt-1;
			delete [] fds;
			fds = fds_tmp;
			fds[cnt].events = ev;
			fds[cnt].fd = fd;
			fds[cnt].revents = 0;
			cnt <<= 1;
	} else {
			for( i=0; i < cnt; i++ )
					if( ! fds[i].events ) {
							fds[i].events = ev;
							fds[i].fd = fd;
							fds[i].revents = 0;
							left--;
							return;
					}
			left = 0;
			add(fd, ev);
	}
}

/**
 *
 */
void cpoll::rm(int fd) {
	for( uint32_t i=0; i<cnt; i++ )
			if( fds[i].fd == fd ) {
					fds[i].fd = -1;
					fds[i].events = 0;
					fds[i].revents = 0;
					left++;
					return;
			}
}

/**
 *
 */
void cpoll::chg(int fd, short ev) {
	add(fd, ev);
}

/**
 * 
 */
int cpoll::poll(int timeout) {
		return ::poll(fds, cnt, timeout);
}

/**
 *
 */
const struct pollfd & cpoll::operator [] (int i) const {
		return fds[i];
}
