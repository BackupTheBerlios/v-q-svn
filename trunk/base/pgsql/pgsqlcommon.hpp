#ifndef __PGSQLCOMMON_HPP
#define __PGSQLCOMMON_HPP

#include <string>
#define HAVE_NAMESPACE_STD 1
#include <pqxx/pqxx>

#define std_try { try
#define std_catch catch( sql_error & e ) { \
	std::string em("sql error: \""); \
	em+=e.query()+"\": "+e.what(); \
	std::cout<<em<<std::endl; \
	throw ::vq::db_error(em.c_str(), __FILE__, __LINE__); \
} catch( std::exception & e ) { \
	cout<<"exception: "<<e.what()<<endl; \
	throw ::vq::except(e.what(), __FILE__, __LINE__); \
} }


#endif
