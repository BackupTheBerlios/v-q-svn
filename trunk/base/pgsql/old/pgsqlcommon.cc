#include "pgsqlcommon.hpp"

using namespace std;

/**
 * Try to find real domain name (may be it's an alias).
 * \param pg connection to database
 * \param dom domain name (will be changed if it's an alias)
 * \return 0 if alias was found, 1 not found
 * \throw std::runtime_error if any error occurs
 */
char alias2dom( pqxx::Connection &pg, string &dom ) {
	pqxx::NonTransaction ip2dom(pg);
	pqxx::Result res(ip2dom.Exec(
		"SELECT domain FROM ip2domain WHERE ip="+pqxx::Quote(dom, false)));
	if( ! res.empty() ) {
			const char * alias = res[0][0].c_str(); 
			if(!alias) return 1; // it's empty
			dom = alias;
			return 0;
	}
	return 1;
}
