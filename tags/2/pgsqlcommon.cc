#include "pgsqlcommon.h"
#include "util.h"

using namespace std;

char alias2dom( PgDatabase * pg, string &dom ) {
    if( pg->Exec(("SELECT domain FROM ip2domain WHERE ip='"
        +escape(dom)+"'").c_str()) != PGRES_TUPLES_OK ) {
            return 2;
    }
    if( pg->Tuples() ) {
            dom = pg->GetValue(0,0); 
            return 0;
    }
    return 1;
}
