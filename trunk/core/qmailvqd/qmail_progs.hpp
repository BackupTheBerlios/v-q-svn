#ifndef __QMAIL_PROGS_HPP
#define __QMAIL_PROGS_HPP

#include <stdexcept>
#include <string>

enum qmail_prog {
		qp_send_restart // run vq/bin/qmail-send-restart
};

std::string qp2prog( qmail_prog ) throw(std::runtime_error);

#endif // ifndef __QMAIL_PROGS_HPP
