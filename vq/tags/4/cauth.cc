#include <cerrno>

#include "cauth.h"

using namespace std;

cauth::cauth() : lr_errno(0), lastret(cvq::err_no), lastret_blkd(false) {
}

/**
 *
 */
cauth::~cauth() {
}

/**
 *
 */
uint8_t cauth::lr( uint8_t ret, const std::string & msg ) { 
		if( ret != cvq::err_no && ret != lastret && ! lastret_blkd ) {
				lastret = ret;
				lr_info = msg;
				lr_errno = errno;
		}
		return ret;
}

/**
*
*/
string cauth::err_info() const {
   return lr_info.c_str();
}

/**
*
*/
int cauth::err_sys() const {
   return lr_errno;
}

/**
*
*/
string cauth::err_report() const {
   return cvq::err_str(lastret)+": "+lr_info+": "+strerror(lr_errno);
}
