all: vquserrm install

clean:
	rm -rf `cat TARGETS`

cgi.o: cgi.cc cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

conf_home.cc: str2cc conf/home
	./str2cc "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc

conf_vq_gid.cc: getgid conf/vq_user
	./getgid "`head -n 1 conf/vq_user`" vq_gid > conf_vq_gid.cc.tmp \
	&& mv conf_vq_gid.cc.tmp conf_vq_gid.cc

conf_vq_gid.o: compile conf_vq_gid.cc conf_vq_gid.h
	./compile conf_vq_gid.cc

conf_vq_uid.cc: getuid conf/vq_user
	./getuid "`head -n 1 conf/vq_user`" vq_uid > conf_vq_uid.cc.tmp \
	&& mv conf_vq_uid.cc.tmp conf_vq_uid.cc

conf_vq_uid.o: compile conf_vq_uid.cc conf_vq_uid.h
	./compile conf_vq_uid.cc 

conf_xmlstart.cc: str2cc conf/xmlstart
	./str2cc "`head -n 1 conf/xmlstart`" xmlstart > conf_xmlstart.cc

conf_xmlstart.o: compile conf_xmlstart.cc conf_xmlstart.h
	./compile conf_xmlstart.cc

conf_xslt_file.cc: str2cc conf/xslt_file
	./str2cc "`head -n 1 conf/xslt_file`" xslt_file a > conf_xslt_file.cc

conf_xslt_file.o: compile conf_xslt_file.cc conf_xslt_file.h
	./compile conf_xslt_file.cc 

conf_xsltsheet.a: make-lib conf_xslt_file.o conf_xsltsheet.o
	./make-lib conf_xsltsheet conf_xslt_file.o

conf_xsltsheet.o: compile conf_xsltsheet.cc conf_xsltsheet.h conf_xslt_file.h \
conf_home.h
	./compile conf_xsltsheet.cc

getgid: getgid.o load
	./load getgid

getgid.o: compile getgid.cc
	./compile getgid.cc

getuid: getuid.o load
	./load getuid

getuid.o: compile getuid.cc
	./compile getuid.cc

html.o: compile html.cc 
	./compile html.cc

install: load install.o conf_home.o conf_vq_uid.o conf_vq_gid.o
	./load install conf_home.o conf_vq_uid.o conf_vq_gid.o

install.o: compile install.cc conf_home.h
	./compile install.cc
	
sabmain.o: compile sabmain.cc conf_xmlstart.h conf_xsltsheet.h sab.inc
	./compile sabmain.cc `cat sab.inc`
 
str2cc: str2cc.o load
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc  

vquserrm: load vquserrm.o vq.lib sabmain.o conf_xsltsheet.a sab.lib \
cgicc.lib cgi.o html.o conf_xmlstart.o conf_home.o
	./load vquserrm sabmain.o cgi.o conf_xsltsheet.a html.o conf_xmlstart.o \
	conf_home.o `cat vq.lib sab.lib cgicc.lib`

vquserrm.o: compile vquserrm.cc vq.inc cgicc.inc cgi.h html.h conf_home.h
	./compile vquserrm.cc `cat vq.inc` `cat cgicc.inc`
