#ifndef __QMAIL_FILES_H
#define __QMAIL_FILES_H

#include <stdexcept>

enum qmail_file {
	qf_assign='a', // users/assign
	qf_rcpthosts, // control/rcpthosts
	qf_morercpthosts, // control/morercpthosts
	qf_locals, // control/locals
	qf_virtualdomains // control/virtualdomains
};

const char *qf2file( qmail_file ) throw(std::runtime_error);

#endif // ifndef __QMAIL_FILES_H
