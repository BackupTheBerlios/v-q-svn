#ifndef __CQMAIL_COMMON_HPP
#define __CQMAIL_COMMON_HPP

#define std_try { try
#define std_catch catch( std::exception & e ) { \
	std::cout<<"exception: "<<e.what()<<std::endl; \
	throw ::vq::except(e.what(), __FILE__, __LINE__); \
} }
	
#endif // ifndef __CQMAIL_COMMON_HPP
