all: libvalid.so install

clean: 
	rm -rf `cat TARGETS`

setup: all
	./install

###################
conf_home.cc: str2cc conf/home
	./str2cc valid "`head -n 1 conf/home`" home a > conf_home.cc

conf_home.o: compile conf_home.cc conf_home.h
	./compile conf_home.cc

hier.o: compile hier.cc install.h
	./compile hier.cc

install: load install.o conf_home.o hier.o
	./load install hier.o conf_home.o

install.o: compile install.cc conf_home.h
	./compile install.cc

libvalid.so: libvalid.so.1 
	ln -sf libvalid.so.1 libvalid.so

libvalid.so.1: make-so valid.o
	./make-so libvalid.so.1 valid.o

str2cc: load str2cc.o
	./load str2cc

str2cc.o: compile str2cc.cc
	./compile str2cc.cc

valid.o: comp-so valid.h valid.cc
	./comp-so valid.cc
