@echo off
REM --------------------------------------------------------------------
REM Copyright IONA Technologies 2004. All rights reserved.
REM This software is provided "as is".
REM --------------------------------------------------------------------
set TCL_SCRIPT=C:/corbautil/bin/orbix_ns_on_fixed_port.tcl
itadmin %TCL_SCRIPT% -exe orbix_ns_on_fixed_port %*
