SUBDIR=tools libconf libmain libsys libtext base interfaces

.PHONY: all clean pgsql install install-pgsql

all clean:
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

install:
	for p in etc etc/ivq etc/iauth etc/ilogger bin lib lib/v-q include include/v-q ; do \
		$(INSTALL) -d -m 0755 $(PREFIX)/$$p ; \
	done
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			exec $(MAKE) $@; \
		) || exit 1; \
	done

