#ifndef __CGI_H
#define __CGI_H

#include <string>
#include <vector>
#include "cgicc/Cgicc.h"

/// returns value of cgi variable
bool cgi_var(const cgicc::Cgicc *, const std::string &, std::string & );
bool cgi_var(const cgicc::Cgicc &, const std::string &, std::string & );

/// returns true if cookie exists
bool cgi_ck( const std::vector<cgicc::HTTPCookie> &, 
		const std::string &, std::string & );

/// returns true if cookie exists
bool cgi_ck( const std::vector<cgicc::HTTPCookie> &, 
		const std::string &, cgicc::HTTPCookie & );

/// returns pointer to cookie, if not found returns NULL
cgicc::HTTPCookie *cgi_ck(std::vector<cgicc::HTTPCookie> &, 
		const std::string & );

/// adds cookies
void cookies_cp( std::vector<cgicc::HTTPCookie> &, 
		const std::vector<cgicc::HTTPCookie> & );

#endif
