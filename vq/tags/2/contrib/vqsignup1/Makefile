all: vqsignup1 install

setup: all
	./install

#######################
cgi.o: compile cgi.cc cgi.h html.h cgicc.inc
	./compile cgi.cc `cat cgicc.inc`

conf-h1: conf/conf_dir
	touch conf-h1

conf_cgi_bin.cc: conf-h1 str2cc conf/cgi_bin
	./conf-h1 conf/cgi_bin | ./str2cc cgi_bin a > conf_cgi_bin.cc

conf_cgi_bin.o: compile conf_cgi_bin.cc conf_cgi_bin.h
	./compile conf_cgi_bin.cc

conf_conf_dir.cc: conf-h1 str2cc conf/conf_dir
	./conf-h1 conf/conf_dir | ./str2cc conf_dir a > conf_conf_dir.cc

conf_conf_dir.o: compile conf_conf_dir.cc conf_conf_dir.h
	./compile conf_conf_dir.cc

conf_domains.cc: conf-h1 str2cc conf/domains
	./conf-h1 conf/domains | ./str2cc domains a > conf_domains.cc

conf_domains.o: compile conf_domains.cc conf_domains.h
	./compile conf_domains.cc 

conf_html_br.cc: conf-h1 str2cc conf/html_br
	./conf-h1 conf/html_br | ./str2cc html_br a > conf_html_br.cc

conf_html_br.o: compile conf_html_br.h conf_html_br.cc
	./compile conf_html_br.cc

conf_notify.cc: conf-h1 str2cc conf/notify
	./conf-h1 conf/notify | ./str2cc notify a > conf_notify.cc

conf_notify.o: compile conf_notify.h conf_notify.cc
	./compile conf_notify.cc
	
conf_pass_ok.cc: conf-h1 str2cc conf/pass_ok
	./conf-h1 conf/pass_ok | ./str2cc pass_ok > conf_pass_ok.cc

conf_pass_ok.o: compile conf_pass_ok.cc conf_pass_ok.h
	./compile conf_pass_ok.cc

conf_pgsql.cc: conf-h1 str2cc conf/pgsql
	./conf-h1 conf/pgsql | ./str2cc pgsql a > conf_pgsql.cc

conf_pgsql.o: compile conf_pgsql.h conf_pgsql.cc
	./compile conf_pgsql.cc

conf_user_ok.cc: conf-h1 str2cc conf/user_ok
	./conf-h1 conf/user_ok | ./str2cc user_ok > conf_user_ok.cc
	
conf_user_ok.o: compile conf_user_ok.cc conf_user_ok.h
	./compile conf_user_ok.cc

conf_vq_gid.cc: conf-h1 getgid int2cc conf/vq_user
	./conf-h1 conf/vq_user | ./getgid | ./int2cc vq_gid > conf_vq_gid.cc.tmp \
	&& mv conf_vq_gid.cc.tmp conf_vq_gid.cc
	
conf_vq_gid.o: compile conf_vq_gid.cc conf_vq_gid.h
	./compile conf_vq_gid.cc

conf_vq_uid.cc: conf-h1 getuid int2cc conf/vq_user
	./conf-h1 conf/vq_user | ./getuid | ./int2cc vq_uid > conf_vq_uid.cc.tmp \
	&& mv conf_vq_uid.cc.tmp conf_vq_uid.cc

conf_vq_uid.o: compile conf_vq_uid.cc conf_vq_uid.h
	./compile conf_vq_uid.cc

conf_xmlstart.cc: conf-h1 str2cc conf/xmlstart
	./conf-h1 conf/xmlstart | ./str2cc xmlstart > conf_xmlstart.cc

conf_xmlstart.o: compile conf_xmlstart.cc conf_xmlstart.h
	./compile conf_xmlstart.cc

conf_xmlstart1.cc: conf-h1 str2cc conf/xmlstart1
	./conf-h1 conf/xmlstart1 | ./str2cc xmlstart1 > conf_xmlstart1.cc

conf_xmlstart1.o: compile conf_xmlstart1.cc conf_xmlstart1.h
	./compile conf_xmlstart1.cc

conf_xslt_dir.cc: conf-h1 str2cc conf/xslt_dir
	./conf-h1 conf/xslt_dir | ./str2cc xslt_dir a > conf_xslt_dir.cc

conf_xslt_dir.o: compile conf_xslt_dir.cc conf_xslt_dir.h
	./compile conf_xslt_dir.cc

conf_xslt_dir1.cc: conf-h1 str2cc conf/xslt_dir1
	./conf-h1 conf/xslt_dir1 | ./str2cc xslt_dir1 a > conf_xslt_dir1.cc

conf_xslt_dir1.o: compile conf_xslt_dir1.cc conf_xslt_dir1.h
	./compile conf_xslt_dir1.cc

conf_xslt_file.cc: conf-h1 str2cc conf/xslt_file
	./conf-h1 conf/xslt_file | ./str2cc xslt_file a > conf_xslt_file.cc

conf_xslt_file.o: compile conf_xslt_file.cc conf_xslt_file.h
	./compile conf_xslt_file.cc

conf_xslt_file1.cc: conf-h1 str2cc conf/xslt_file1
	./conf-h1 conf/xslt_file1 | ./str2cc xslt_file1 a > conf_xslt_file1.cc

conf_xslt_file1.o: compile conf_xslt_file1.cc conf_xslt_file1.h
	./compile conf_xslt_file1.cc

conf_xsltsheet.a: make-lib conf_xslt_dir.o conf_xslt_file.o conf_xsltsheet.o
	./make-lib conf_xsltsheet conf_xslt_dir.o conf_xslt_file.o

conf_xsltsheet.o: compile conf_xsltsheet.cc conf_xsltsheet.h conf_xslt_file.h \
conf_xslt_dir.h
	./compile conf_xsltsheet.cc

conf_xsltsheet1.a: make-lib conf_xslt_dir1.o conf_xslt_file1.o conf_xsltsheet1.o
	./make-lib conf_xsltsheet1 conf_xslt_dir1.o conf_xslt_file1.o

conf_xsltsheet1.o: compile conf_xsltsheet1.cc conf_xsltsheet1.h conf_xslt_file1.h \
conf_xslt_dir1.h
	./compile conf_xsltsheet1.cc

getgid: load getgid.o
	./load getgid

getgid.o: compile getgid.cc
	./compile getgid.cc

getuid: load getuid.o
	./load getuid

getuid.o: compile getuid.cc
	./compile getuid.cc

html.o: compile html.cc html.h
	./compile html.cc

install: load install.o conf_vq_uid.o conf_vq_gid.o conf_xsltsheet.a \
conf_cgi_bin.o conf_conf_dir.o conf_xsltsheet1.a
	./load install conf_vq_uid.o conf_vq_gid.o conf_xsltsheet.a \
	conf_cgi_bin.o conf_conf_dir.o conf_xsltsheet1.a
	
install.o: compile install.cc conf_conf_dir.h conf_cgi_bin.h \
conf_xslt_dir.h conf_xslt_dir1.h conf_vq_uid.h conf_vq_gid.h
	./compile install.cc

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

vqsignup1: load vqsignup1.o sabmain.o sab.lib conf_xmlstart.o \
conf_xsltsheet.a cgi.o cgicc.lib conf_domains.o conf_user_ok.o \
conf_pass_ok.o conf_html_br.o html.o vq.lib valid.lib conf_pgsql.o pgsql.lib \
conf_notify.o
	./load vqsignup1 sabmain.o `cat sab.lib` conf_xmlstart.o \
    conf_xsltsheet.a cgi.o `cat cgicc.lib` conf_domains.o \
	conf_user_ok.o conf_pass_ok.o conf_html_br.o html.o conf_pgsql.o \
	conf_notify.o \
	`cat vq.lib valid.lib pgsql.lib`

vqsignup1.o: compile vqsignup1.cc cgicc.inc conf_domains.h conf_user_ok.h \
conf_pass_ok.h conf_html_br.h html.h vq.inc valid.inc conf_pgsql.h pgsql.inc \
conf_notify.h
	./compile vqsignup1.cc `cat cgicc.inc vq.inc valid.inc pgsql.inc`

vqsignup1_notify: load vqsignup1_notify.o sab.lib conf_xmlstart1.o \
conf_xsltsheet1.a conf_pgsql.o html.o pgsql.lib vq.lib
	./load vqsignup1_notify conf_xmlstart1.o conf_xsltsheet1.a conf_pgsql.o \
	html.o `cat sab.lib pgsql.lib vq.lib` 

vqsignup1_notify.o: compile vqsignup1_notify.cc conf_xmlstart.h \
conf_xsltsheet.h conf_pgsql.h html.h sab.inc pgsql.inc vq.inc
	./compile vqsignup1_notify.cc `cat sab.inc pgsql.inc vq.inc`

#####################
clean:
		rm -rf `cat TARGETS`
