BASE=../..

.PHONY: auto-mkdir

all: auto-mkdir .depend daemon

#######################
auto-mkdir:
	mkdir -p auto

auto/dl.lib: try/dl.c $(BASE)/compile $(BASE)/load
	( cd try && $(BASE)/../compile dl.c && $(BASE)/../load dl -ldl >/dev/null 2>&1 \
	&& echo -ldl || echo "" ) > auto/dl.lib
	rm -f try/dl.o try/dl

cforkmaster.o: $(BASE)/compile all.inc
	$(BASE)/compile `cat all.inc` cforkmaster.cc

daemon: $(BASE)/load $(BASE)/main.o daemon.o cforkmaster.o auto/dl.lib all.lib
	$(BASE)/load daemon $(BASE)/main.o cforkmaster.o `cat auto/dl.lib` `cat all.lib`

daemon.o: $(BASE)/compile all.inc
	$(BASE)/compile `cat all.inc` daemon.cc

#####################
.depend: all.inc
	echo '# Module dependencies' > .depend
	$(BASE)/tools/mkdep *.cc `cat all.inc` >> .depend
	
clean:
	rm -rf `cat TARGETS`
