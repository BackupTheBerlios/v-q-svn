#ifndef __HTML_H
#define __HTML_H

#include <stdexcept>
#include <string>

bool htmlbad( std::string &, const char *, const char ) throw(std::logic_error);
std::string html_spec_enc( const std::string & ) throw();
std::string html_spec_dec( const std::string & ) throw();

#endif
