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

install-conf: install-dirs
	@echo "Installing default configuration..."

pkg-tar:
	rm -rf tmp/*
	svn export . tmp/v-q-$(VERSION)
	sed -e "s/@VERSION@/$(VERSION)/" < tmp/v-q-$(VERSION)/pkg/rpm/v-q.spec.in > tmp/v-q-$(VERSION)/pkg/rpm/v-q.spec
	cd tmp && tar -cf v-q-$(VERSION).tar v-q-$(VERSION)
	rm -rf tmp/v-q-$(VERSION)

pkg-tgz: pkg-tar
	cd tmp && gzip -9 v-q-$(VERSION).tar

pkg-tbz2: pkg-tar
	cd tmp && bzip2 -9 v-q-$(VERSION).tar

pkg-rpm: pkg-tgz
	@if [ `id -u` != 0 ] ; then echo You must be root. ; exit 1; fi
	rpmbuild -ta --with jdbc tmp/v-q-$(VERSION).tar.gz

include Makefile.common
