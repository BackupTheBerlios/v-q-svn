#ifndef __SABMAIN_H
#define __SABMAIN_H

#include <string>
#include <map>
#include <iostream>

typedef std::map<std::string, std::string> xslt_args_type;

void start( int, char **, std::ostream &, std::ostream &, xslt_args_type & );

#endif
