all: host_add

#######################
cgi.o: compile cgi.cc cgi.h html.h cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

conf_domains.cc: str2cc conf/domains
	head -n 1 conf/domains | ./str2cc domains a > conf_domains.cc

conf_domains.o: compile conf_domains.cc conf_domains.h
	./compile conf_domains.cc 

conf_home.cc: str2cc conf/home
	head -n 1 conf/home | ./str2cc home a > conf_home.cc

conf_home.o: compile conf_home.cc
	./compile conf_home.cc

conf_html_br.cc: str2cc conf/html_br
	head -n 1 conf/html_br | ./str2cc html_br a > conf_html_br.cc

conf_html_br.o: compile conf_html_br.h conf_html_br.cc
	./compile conf_html_br.cc

conf_pass_ok.cc: str2cc conf/pass_ok
	head -n 1 conf/pass_ok | ./str2cc pass_ok a > conf_pass_ok.cc

conf_pass_ok.o: compile conf_pass_ok.cc conf_pass_ok.h
	./compile conf_pass_ok.cc
	
conf_pref_ok.cc: str2cc conf/pref_ok
	head -n 1 conf/pref_ok | ./str2cc pref_ok a > conf_pref_ok.cc

conf_pref_ok.o: compile conf_pref_ok.cc conf_pref_ok.h
	./compile conf_pref_ok.cc
	
conf_user_ok.cc: str2cc conf/user_ok
	head -n 1 conf/user_ok | ./str2cc user_ok a > conf_user_ok.cc

conf_user_ok.o: compile conf_user_ok.cc conf_user_ok.h
	./compile conf_user_ok.cc

conf_vq_gid.cc: getgid int2cc conf/vq_user
	head -n 1 conf/vq_user | ./getgid | ./int2cc vq_gid > conf_vq_gid.cc
	
conf_vq_gid.o: compile conf_vq_gid.cc conf_vq_gid.h
	./compile conf_vq_gid.cc

conf_vq_uid.cc: getuid int2cc conf/vq_user
	head -n 1 conf/vq_user | ./getuid | ./int2cc vq_uid > conf_vq_uid.cc

conf_vq_uid.o: compile conf_vq_uid.cc conf_vq_uid.h
	./compile conf_vq_uid.cc

conf_xmlstart.cc: str2cc conf/xmlstart
	head -n 1 conf/xmlstart | ./str2cc xmlstart > conf_xmlstart.cc

conf_xmlstart.o: compile conf_xmlstart.cc conf_xmlstart.h
	./compile conf_xmlstart.cc

conf_xsltsheet.cc: str2cc conf/xsltsheet
	head -n 1 conf/xsltsheet | ./str2cc xsltsheet a > conf_xsltsheet.cc

conf_xsltsheet.o: compile conf_xsltsheet.cc conf_xsltsheet.h
	./compile conf_xsltsheet.cc

html.o: compile html.cc html.h
	./compile html.cc

int2cc: int2cc.o load
	./load int2cc

int2cc.o: compile int2cc.cc
	./compile int2cc.cc

sabmain.o: compile sabmain.cc conf_xmlstart.h conf_xsltsheet.h sab.inc
	./compile sabmain.cc `cat sab.inc`

str2cc: str2cc.o load
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

getgid: load getgid.o
	./load getgid

getgid.o: compile getgid.cc
	./compile getgid.cc
	
getuid: load getuid.o
	./load getuid

getuid.o: compile getuid.cc
	./compile getuid.cc

install: all load install.o conf_home.o conf_vq_uid.o conf_vq_gid.o
	./load install conf_home.o conf_vq_uid.o conf_vq_gid.o
	./install

install.o: compile install.cc conf_home.h conf_vq_uid.h conf_vq_gid.h
	./compile install.cc

host_add: load host_add.o sabmain.o sab.lib conf_xmlstart.o \
conf_xsltsheet.o cgi.o cgicc.lib conf_domains.o conf_user_ok.o \
conf_pass_ok.o conf_html_br.o html.o conf_pref_ok.o
	./load host_add sabmain.o `cat sab.lib` conf_xmlstart.o \
    conf_xsltsheet.o cgi.o `cat cgicc.lib` conf_domains.o \
	conf_user_ok.o conf_pass_ok.o conf_html_br.o html.o \
	conf_pref_ok.o \
	../../vq_init.o ../../cfsvq.a ../../cdaemonauth.a ../../split.a \
	../../util.o

host_add.o: compile host_add.cc cgicc.inc conf_domains.h conf_user_ok.h \
conf_pass_ok.h conf_html_br.h html.h conf_pref_ok.h ../../vq_init.h
	./compile host_add.cc `cat cgicc.inc` -I../..

#####################
clean:
		rm -rf `cat TARGETS`
