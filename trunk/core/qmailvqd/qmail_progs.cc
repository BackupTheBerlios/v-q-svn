#include "qmail_progs.hpp"

std::string qp2prog( qmail_prog qp ) throw(std::runtime_error) {
	switch(qp) {
	case qp_send_restart:
			return "/bin/qmail-send-restart";
	default:
			throw std::runtime_error("qp2prog: invalid code");
	}
}
