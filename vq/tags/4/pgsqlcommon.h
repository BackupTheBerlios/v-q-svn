#ifndef __PGSQLCOMMON_H
#define __PGSQLCOMMON_H

#include <string>
#define HAVE_NAMESPACE_STD 1
#include <pqxx/all.h>

/// translates alias to domain name
char alias2dom( pqxx::Connection &, std::string & );

#endif
