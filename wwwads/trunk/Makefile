all: wwwads wwwads-conf adsd

conf_adsalt.cc: str2cc conf/adsalt
	head -n 1 conf/adsalt | ./str2cc adsalt a > conf_adsalt.cc

conf_adsalt.o: compile conf_adsalt.cc
	./compile conf_adsalt.cc

conf_adsdir.cc: str2cc conf/adsdir
	head -n 1 conf/adsdir | ./str2cc adsdir a > conf_adsdir.cc

conf_adsdir.o: compile conf_adsdir.cc
	./compile conf_adsdir.cc

conf_adswwwdir.cc: str2cc conf/adswwwdir
	head -n 1 conf/adswwwdir | ./str2cc adswwwdir a > conf_adswwwdir.cc
	
conf_adswwwdir.o: compile conf_adswwwdir.cc
	./compile conf_adswwwdir.cc

conf_asisext.cc: str2cc conf/asisext
	head -n 1 conf/asisext | ./str2cc asisext a > conf_asisext.cc

conf_asisext.o: compile conf_asisext.cc
	./compile conf_asisext.cc

conf_htmlbadrep.cc: str2cc conf/htmlbadrep
	head -n 1 conf/htmlbadrep | ./str2cc htmlbadrep a >conf_htmlbadrep.cc

conf_htmlbadrep.o: compile conf_htmlbadrep.cc
	./compile conf_htmlbadrep.cc
	
conf_htmlok.cc: str2cc conf/htmlok
	head -n 1 conf/htmlok | ./str2cc htmlok a > conf_htmlok.cc

conf_htmlok.o: compile conf_htmlok.cc
	./compile conf_htmlok.cc

conf_socket_mode.cc: int2cc conf/socket_mode
	head -n 1 conf/socket_mode | ./int2cc socket_mode > conf_socket_mode.cc

conf_socket_mode.o: compile conf_socket_mode.cc
	./compile conf_socket_mode.cc

conf_wwwdir.cc: str2cc conf/wwwdir
	head -n 1 conf/wwwdir | ./str2cc wwwdir a > conf_wwwdir.cc

conf_wwwdir.o: compile conf_wwwdir.cc
	./compile conf_wwwdir.cc

conf_xmlstart.cc: str2cc conf/xmlstart
	head -n 1 conf/xmlstart | ./str2cc xmlstart > conf_xmlstart.cc

conf_xmlstart.o: compile conf_xmlstart.cc
	./compile conf_xmlstart.cc

conf_xsltsheet.cc: str2cc conf/xsltsheet
	head -n 1 conf/xsltsheet | ./str2cc xsltsheet a > conf_xsltsheet.cc

conf_xsltsheet.o: compile conf_xsltsheet.cc
	./compile conf_xsltsheet.cc

dirs.o: compile dirs.cc
	./compile dirs.cc

int2cc: load int2cc.o
	./load int2cc

int2cc.o: int2cc.cc
	./compile int2cc.cc

sabmain.o: compile sabmain.cc sab.inc conf_xmlstart.h conf_xsltsheet.h
	./compile sabmain.cc `cat sab.inc`

wwwads: wwwads.o getline.o fd.o sig.o fdstr.o cgicc.lib conf_adsdir.o \
conf_wwwdir.o conf_adsalt.o
	./load wwwads getline.o fd.o sig.o fdstr.o conf_adsdir.o \
	conf_wwwdir.o conf_adsalt.o `cat cgicc.lib`

wwwads.o: conf_adsdir.h adsd.h getline.h fd.h sig.h conf_wwwdir.h fdstr.h \
cgicc.inc conf_adsalt.h
	./compile wwwads.cc `cat cgicc.inc`
	
wwwads-conf: wwwads-conf.o getline.o is_uint32_t.o html.o fd.o sig.o \
fdstr.o conf_xmlstart.o conf_xsltsheet.o cgicc.lib sab.lib \
conf_htmlok.o conf_htmlbadrep.o conf_adsdir.o conf_wwwdir.o sabmain.o
	./load wwwads-conf getline.o is_uint32_t.o html.o fd.o sig.o fdstr.o \
	conf_xmlstart.o conf_xsltsheet.o conf_htmlok.o conf_htmlbadrep.o \
	conf_adsdir.o conf_wwwdir.o sabmain.o \
	`cat cgicc.lib sab.lib`

wwwads-conf.o: conf_adsdir.h adsd.h getline.h is_uint32_t.h \
html.h fd.h sig.h fdstr.h cgicc.inc sab.inc \
conf_wwwdir.h conf_htmlok.h conf_htmlbadrep.h sabmain.h
	./compile wwwads-conf.cc `cat cgicc.inc sab.inc`

cdiskads.o: cads.h cdiskads.h util.h fdstr.h dirs.h
	./compile cdiskads.cc

cdiskads.a: make-lib cdiskads.o conf_asisext.o
	./make-lib cdiskads conf_asisext.o

cdiskadstest.o: cdiskads.h
	./compile cdistadstest.cc

adsd: adsd.o cdiskads.a getline.o util.o sig.o fd.o fdstr.o \
conf_adsdir.o conf_adswwwdir.o dirs.o conf_socket_mode.o
	./load adsd cdiskads.a getline.o util.o sig.o fd.o fdstr.o \
	conf_adsdir.o conf_adswwwdir.o dirs.o conf_socket_mode.o

adsd.o: adsd.h cdiskads.h getline.h sig.h fd.h conf_adsd.h fdstr.h \
conf_adsdir.h conf_adswwwdir.h conf_socket_mode.h
	./compile adsd.cc

getline.o: getline.h
	./compile getline.cc
	
is_uint32_t.o: is_uint32_t.h
	./compile is_uint32_t.cc
	
test.o: cdiskadstest.h
	./test.cc
	
util.o: compile util.h
	./compile util.cc
	
html.o: compile html.h
	./compile html.cc
	
sig.o: compile sig.h
	./compile sig.cc

str2cc: load str2cc.o
	./load str2cc

str2cc.o: str2cc.cc
	./compile str2cc.cc

fd.o: compile fd.h
	./compile fd.cc
	
fdstr.o: compile fdstr.h fd.h
	./compile fdstr.cc

clean:
		rm -rf `cat TARGETS`
