#!/bin/sh

for p in $(find . -type f -iname "*.html") ; do
	mt=$(svn pg svn:mime-type $p)
	if [ X"$mt" != X"text/html" ] ; then
		svn ps svn:mime-type text/html $p
	fi
done
