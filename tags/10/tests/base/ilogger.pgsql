#!/bin/sh
$BASE/base/pgsql/pgsqllogd \
-ORBInitRef NameService=corbaloc::$host:$port/NameService -x "name_service#`basename $0`" . &
echo $! > run.pid
