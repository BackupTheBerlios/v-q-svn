SUBDIR=tools libconf libmain libsys libtext base interfaces

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

install-dirs:
	for p in etc etc/ivq etc/iauth etc/ilogger bin lib lib/vq include include/vq ; do \
		$(INSTALL) -d -m 0755 $(PREFIX)/$$p ; \
	done

install-conf: install-dirs
	@echo "Installing default configuration..."
	@if [ ! -f $(PREFIX)/etc/ivq/orb_conf ] ; then \
		echo "-ORBInitRef NameService=corbaloc::127.0.0.1:12456/NameService" > $(PREFIX)/etc/ivq/orb_conf ; \
		chmod 644 $(PREFIX)/etc/ivq/orb_conf ; \
		echo "$(PREFIX)/etc/ivq/orb_conf: created" ; \
	fi

include Makefile.common
