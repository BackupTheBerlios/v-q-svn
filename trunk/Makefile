SUBDIR=tools libconf libmain libvqmain libsys libtext core

.PHONY: all clean pgsql

all::
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			$(MAKE) $@; \
		); \
	done

pgsql::
	@for entry in core/pgsql; do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			$(MAKE) $@; \
		); \
	done

clean:
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			$(MAKE) $@; \
		); \
	done

