#ifndef __PGSQLCOMMON_H
#define __PGSQLCOMMON_H

#include <string>
#include <pg_config.h>
#include <libpq++.h>

/**
 * translates alias to domain name
 * \return 0 on success, 1 no such alias, 2 sql error
 */
char alias2dom( PgDatabase *, std::string & );

#endif
