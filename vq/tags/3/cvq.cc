#include <sstream>

#include "cvq.h"

using namespace std;

cvq::logic_error::logic_error( const std::string &w, const std::string &f,
		int l) {
	ostringstream os;
	os<<f<<'@'<<l<<':'<<w;
	_what = os.str();
}

const char * cvq::logic_error::what() const throw() {
	return _what.c_str();
}

cvq::runtime_error::runtime_error( const std::string &w, const std::string &f,
		int l) {
	ostringstream os;
	os<<f<<'@'<<l<<':'<<w;
	_what = os.str();
}

const char * cvq::runtime_error::what() const throw() {
	return _what.c_str();
}
