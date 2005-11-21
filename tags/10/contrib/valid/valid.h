#ifndef __VALID_H
#define __VALID_H

#include <string>

namespace valid {

/// polish tax identification number
std::string pl_nip_valid(const std::string &); 

/// polish post code
std::string pl_pcode_valid(const std::string &);

/// e-mail
std::string email_valid(const std::string &);

/// date in format yyyy-mm-dd
std::string date_valid(std::string);

} 

#endif
