#ifndef __CGI_H
#define __CGI_H

#include <string>
#include <vector>
#include "cgicc/Cgicc.h"

using namespace std;
using namespace cgicc;

bool cgi_var(Cgicc *, const string &, string & );
bool cgi_ck( const vector<HTTPCookie> *, const string &, string & );

#endif
