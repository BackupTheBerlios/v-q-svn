#!/bin/sh
echo "starting iauth(pgsqlauthd) server ..."
$BASE/base/pgsql/pgsqlauthd \
-ORBInitRef NameService=corbaloc::$host:$port/NameService -x "name_service#`basename $0`" . &
echo $! > run.pid
