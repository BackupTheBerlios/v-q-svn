#ifndef __CGI_H
#define __CGI_H

#include <string>
#include <vector>
#include "cgicc/Cgicc.h"

using namespace cgicc;

/// returns value of cgi variable
bool cgi_var(Cgicc *, const std::string &, std::string & );
/// returns true if cookie exists
bool cgi_ck( const vector<HTTPCookie> &, const std::string &, std::string & );
/// returns true if cookie exists
bool cgi_ck( const vector<HTTPCookie> &, const std::string &, HTTPCookie & );
/// returns pointer to cookie, if not found returns NULL
HTTPCookie *cgi_ck(std::vector<HTTPCookie> &, const std::string & ) throw();
/// adds cookies
void cookies_cp( std::vector<HTTPCookie> &, const std::vector<HTTPCookie> & ) throw ();

#endif
