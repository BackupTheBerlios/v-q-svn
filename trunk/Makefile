SUBDIR=tools libconf libmain libsys libtext corbautil base interfaces
# First try BSD
VERSION!=head -n 1 VERSION
# If it doesn't work try GNU
VERSION?=$(shell head -n 1 VERSION)

.PHONY: all clean pgsql install install-pgsql install-conf install-dirs

all:

install: install-dirs install-conf

all clean install:
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			exec $(MAKE) $@; \
		) || exit 1; \
	done

pgsql install-pgsql:
	@for entry in base/pgsql; do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			exec $(MAKE) $@; \
		) || exit 1; \
	done

jdbc install-jdbc:
	@for entry in base/jdbc; do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			exec $(MAKE) $@; \
		) || exit 1; \
	done

install-dirs:
	for p in etc etc/ivq etc/iauth etc/ilogger bin lib lib/vq include include/vq ; do \
		$(INSTALL) -d -m 0755 $(PREFIX)/$$p ; \
	done

install-conf: install-dirs
	@echo "Installing default configuration..."

pkg-tgz:
	rm -rf tmp/*
	svn export . tmp/v-q-$(VERSION)
	sed -e "s/@VERSION@/$(VERSION)/" < tmp/v-q-$(VERSION)/pkg/rpm/v-q.spec.in > tmp/v-q-$(VERSION)/pkg/rpm/v-q.spec
	cd tmp && tar -czf v-q-$(VERSION).tar.gz v-q-$(VERSION)
	rm -rf tmp/v-q-$(VERSION)

pkg-rpm: pkg-tgz
	@if [ `id -u` != 0 ] ; then echo You must be root. ; exit 1; fi
	rpmbuild -ta tmp/v-q-$(VERSION).tar.gz

include Makefile.common
