#ifndef __CQMAIL_COMMON_HPP
#define __CQMAIL_COMMON_HPP

#include <p_orb.h>

#include <iostream>

namespace vq {

	void vq::report_exception( CORBA::SystemException & e );
	void vq::report_exception( CORBA::Exception & e );
	
}

#define std_try { try
#define std_catch catch( CORBA::SystemException & e ) { \
	std::cerr<<"Exception during a call to authorization inteface: "; \
	vq::report_exception(e); \
	this->shutting = true; \
	throw; \
} catch( CORBA::Exception & e ) { \
	std::cerr<<"Exception during a call to authorization inteface: "; \
	vq::report_exception(e); \
	this->shutting = true; \
	throw; \
} catch( std::exception & e ) { \
	std::cerr<<"exception: "<<e.what()<<std::endl; \
	throw ::vq::except(e.what(), __FILE__, __LINE__); \
} }


#endif // ifndef __CQMAIL_COMMON_HPP
