#ifndef __QMAIL_FILES_HPP
#define __QMAIL_FILES_HPP

#include <stdexcept>

enum qmail_file {
	qf_assign='a', // users/assign
	qf_rcpthosts, // control/rcpthosts
	qf_morercpthosts, // control/morercpthosts
	qf_locals, // control/locals
	qf_virtualdomains, // control/virtualdomains
	qf_moreipme // control/moreipme
};

const char *qf2file( qmail_file );

#endif // ifndef __QMAIL_FILES_HPP
