BASE=../..

.PHONY: auto-mkdir depend

all: auto-mkdir daemon

depend:
	@rm -f .depend
	mkdep `cat all.inc` *.cc

#######################
auto-mkdir:
	mkdir -p auto

auto/dl.lib: try/dl.c $(BASE)/compile $(BASE)/load
	( cd try && $(BASE)/../compile dl.c && $(BASE)/../load dl -ldl >/dev/null 2>&1 \
	&& echo -ldl || echo "" ) > auto/dl.lib
	rm -f try/dl.o try/dl

cforkmaster.o: $(BASE)/compile all.inc
	$(BASE)/compile `cat all.inc` cforkmaster.cc

cforkmaster.o: cforkmaster.cc cforkmaster.hpp cdaemonmaster.hpp \
 cdaemonchild.hpp ../libsys/sys.hpp \
 ../libsys/uniq.hpp ../libsys/sig.hpp ../libsys/cpoll.hpp \
 ../libsys/fd.hpp \
 ../libsys/fdstr.hpp ../libsys/getline.hpp ../libsys/util.hpp \
 ../libsys/lock.hpp ../libsys/dirs.hpp

daemon: $(BASE)/load $(BASE)/main.o daemon.o cforkmaster.o auto/dl.lib all.lib
	$(BASE)/load daemon $(BASE)/main.o cforkmaster.o `cat auto/dl.lib` `cat all.lib`

daemon.o: $(BASE)/compile all.inc
	$(BASE)/compile `cat all.inc` daemon.cc

daemon.o: daemon.cc ../../main.hpp cdaemonmaster.hpp cdaemonchild.hpp \
 ../libsys/sys.hpp \
 ../libsys/uniq.hpp ../libsys/sig.hpp ../libsys/cpoll.hpp \
 ../libsys/fd.hpp \
 ../libsys/fdstr.hpp ../libsys/getline.hpp ../libsys/util.hpp \
 ../libsys/lock.hpp ../libsys/dirs.hpp

#####################
clean:
		rm -rf `cat TARGETS`
