SUBDIR=tools libconf libmain libvqmain libsys libtext base interfaces

.PHONY: all clean pgsql

all::
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			exec $(MAKE) $@; \
		) || exit 1; \
	done

pgsql::
	@for entry in base/pgsql; do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			exec $(MAKE) $@; \
		) || exit 1; \
	done

clean:
	@for entry in tests $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			exec $(MAKE) $@; \
		) || exit 1; \
	done

