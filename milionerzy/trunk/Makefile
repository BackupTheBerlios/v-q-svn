CXX = g++
SRCS = milionerzy.cc tpytania.cc tstan.cc twyniki.cc
PROG = milionerzy
PREFIX = /usr/local
CXXOPTS += $(CFLAGS) -g -Wall -DSHARE_DIR=\"$(PREFIX)/share/$(PROG)\"

all: $(SRCS:.cc=.o)
		$(CXX) $(LDOPTS) -o $(PROG) $^
        
%.o: %.cc
		$(CXX) $(CXXOPTS) -c $<

tpytania.o: tpytania.h blad.h tstan.h
milionerzy.o: tpytania.h tstan.h blad.h twyniki.h
tstan.o: tstan.h blad.h
twyniki.o: twyniki.h blad.h

clean:
		rm -f $(SRCS:.cc=.o) $(PROG)

install: all
		for d in bin doc doc/$(PROG) share share/$(PROG); do \
			install -d -m 755 $(PREFIX)/$$d ; \
		done
		for d in README TODO; do \
			install -m 644 $$d $(PREFIX)/doc/$(PROG); \
		done 
		cp -R share/* $(PREFIX)/share/$(PROG)
		find $(PREFIX)/share/$(PROG) -type d -exec chmod 755 {} \; -or -type f -exec chmod 644 {} \;
		for d in 1000 32000; do \
		        chmod +t $(PREFIX)/share/$(PROG)/$$d ; \
		done
		touch $(PREFIX)/share/$(PROG)/wyniki
	        chmod 666 $(PREFIX)/share/$(PROG)/wyniki
		install -m 755 $(PROG) $(PREFIX)/bin
