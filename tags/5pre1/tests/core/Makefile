SUBDIR=ivq iauth qmailvqd

.PHONY: all clean

all::
	@for entry in $(SUBDIR); do \
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

