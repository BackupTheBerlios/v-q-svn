#-----------------------------------------------------------------------
# Copyright IONA Technologies 2002-2003.  All rights reserved.
# This software is provided "as is".
#-----------------------------------------------------------------------

!include "..\Makefile.inc"

#--------
# Lists of files used by make rules.
#--------
LIB_OBJ =	PoaUtility\PoaUtility.obj \
		import_export\import_export.obj

#--------
# Rules
#--------

default:	all

all:		..\corbautil.lib

..\corbautil.lib:	make_in_subdirs
	LIB /out:..\corbautil.lib $(LIB_OBJ)

make_in_subdirs:
	cd PoaUtility
	$(MAKE)
	cd ../import_export
	$(MAKE)
	cd ..

clean:
	cd PoaUtility
	$(MAKE) clean
	cd ../import_export
	$(MAKE) clean
	cd ..
	-del ..\*.lib
