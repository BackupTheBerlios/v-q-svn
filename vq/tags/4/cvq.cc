#include <sstream>
#include <cerrno>

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

/**
 *
 */
cvq::cvq() : lr_errno(0), lastret(err_no), lastret_blkd(false) {
}

/**
@return 1 on valid domain name, 0 otherwise
\note I assume specific order of characters in code page.
*/
uint8_t cvq::dom_val(const string & d)
{
	const char *ptr = d.c_str();
	if( d.empty() ) return 0;
	for( ; *ptr; ptr++ ) {
			if( ! ( ( *ptr >= 'A' && *ptr <= 'Z' )
				|| ( *ptr >= 'a' && *ptr <= 'z' )
				|| ( *ptr >= '0' && *ptr <='9' )
				|| *ptr == '.' || *ptr == '-' ) )
					return 0;
	}
	return 1;
}

/**
@return 1 if user name is valid, 0 otherwise
*/
uint8_t cvq::user_val(const string &u)
{
	if( u.empty() ) return 0;
	const char *ptr = u.c_str();
	for( ; *ptr; ptr++ ) {
			if( (*ptr >= 'a' && *ptr <= 'z')
				|| (*ptr >= 'A' && *ptr <= 'Z' )
				|| (*ptr >= '0' && *ptr <= '9' ) )
					continue;

			switch(*ptr) {
			case '$': case '%': case '&': case '\'':
			case '*': case '+': case '-': case '/':
			case '=': case '?': case '^': case '_':
			case '`': case '{': case '|': case '}':
			case '~': case '.': case '!': case '#':
					continue;
			default: 
					return 0;
			}
	}
	return 1;
}

/**
 *
 */
string cvq::err_str( uint8_t e ) {
	return err_str( static_cast<error>(e) );
}

/**
 *
 */
string cvq::err_str( error e ) {
	switch(e) {
	case err_no:
			return "Success";
	case err_temp:
			return "Temporary error";
	case err_dom_inv:
			return "Invalid domain name";
	case err_user_inv:
			return "Invalid user name";
	case err_user_nf:
			return "No such user";
	case err_auth:
			return "Error in authorization module";
	case err_udot_tns:
			return "Type's not supported";
	case err_open:
			return "Open failed";
	case err_wr:
			return "Write failed";
	case err_rd:
			return "Read failed";
	case err_stat:
			return "Stat failed";
	case err_notdir:
			return "Not a directory";
	case err_chmod:
			return "Chmod failed";
	case err_lckd:
			return "It's locked";
	case err_ren:
			return "Rename failed";
	case err_chown:
			return "Chown failed";
	case err_over:
			return "Overflow";
	case err_exec:
			return "Child crashed or returned something unexpected";
	case err_mkdir:
			return "Mkdir failed";
	case err_exists:
			return "Entry exists";
	case err_noent:
			return "Entry does not exist";
	default:
			return "Unknown error";
	}
}

/**
 *
 */
string cvq::err_info() const {
	return lr_info.c_str();
}

/**
 *
 */
uint8_t cvq::lr( uint8_t ret, const std::string & msg ) { 
		if( ret != err_no && ret != lastret && ! lastret_blkd ) {
				lastret = ret;
				lr_info = msg;
				lr_errno = errno;
		}
		return ret;
}

/**
 *
 */
int cvq::err_sys() const {
	return lr_errno;
}

/**
 *
 */
string cvq::err_report() const {
	return err_str(lastret)+"# "+lr_info+"# "+strerror(lr_errno);
}
