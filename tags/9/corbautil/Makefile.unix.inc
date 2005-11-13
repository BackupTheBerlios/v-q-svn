INSTALL?=install

.SUFFIXES: .cxx

.cxx.o: $(BASE)/corba.inc
	$(BASE)/comp-so $< -I. -I../gsp -I../import_export -I../PoaUtility \
	-I../portability `cat $(BASE)/corba.inc`

include $(BASE)/Makefile.common
