all: mkdep

mkdep::
	# mkdepend is from MICO
	-which mkdepend 2>&1 > /dev/null; \
	if [ "X$${?}" = X"0" ] ; then \
		ln -sf `which mkdepend` ./mkdep ; \
	fi
	which mkdep 2>&1 > /dev/null; \
	if [ "X$${?}" = X"0" ] ; then \
		ln -sf `which mkdep` ./mkdep ; \
	fi

