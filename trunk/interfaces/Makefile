SUBDIR=clue

.PHONY: all clean install

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

install: all
	@for entry in $(SUBDIR); do \
		( \
			echo "===> $$entry" ; \
			cd $$entry ; \
			$(MAKE) $@; \
		); \
	done

