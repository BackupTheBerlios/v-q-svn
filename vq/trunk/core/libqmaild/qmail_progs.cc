#include "qmail_progs.h"
#include "conf_home.h"
#include "vq_conf.h"

using vq::conf_home;

std::string qp2prog( qmail_prog qp ) throw(std::runtime_error) {
	switch(qp) {
	case qp_send_restart:
			return conf_home+"/bin/qmail-send-restart";
	default:
			throw std::runtime_error("qp2prog: invalid code");
	}
}
