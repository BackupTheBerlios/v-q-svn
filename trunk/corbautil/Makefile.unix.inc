P_USE_ORB?=P_USE_MICO

.SUFFIXES: .cxx

.cxx.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $< -I. \
	-I../gsp \
	-I../import_export \
	-I../PoaUtility \
	-I../portability -D$(P_USE_ORB)

