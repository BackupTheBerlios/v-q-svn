#ifndef __HTML_H
#define __HTML_H

#include <stdexcept>
#include <string>

bool htmlbad( std::string &, const char *, const char );
std::string html_spec_enc( const std::string & );
std::string html_spec_dec( const std::string & );

#endif
