#include "cgicc/Cgicc.h"
#include "cgi.h"

using namespace std;
using namespace cgicc;

/**
 * return value of specified cgi variable
 */
bool cgi_var( Cgicc *cgi, const string &name, string &val ) {
    const_form_iterator fi = cgi->getElement(name);
    if( fi != cgi->getElements().end() ) {
            val = fi->getValue();
            return true;
    }
    val = "";
    return false;
}

/**
 * return value of specified cookie
 */
bool cgi_ck( const vector<HTTPCookie> & cks, const string & ck, string & v ) {
    vector<HTTPCookie>::const_iterator ptr = cks.begin(), end = cks.end();
    for( ; ptr!=end; ptr++ ) {
            if( stringsAreEqual(ptr->getName(), ck) ) {
                    v = ptr->getValue();
                    return true;
            }
    }
    v = "";
    return false;
}

/**
 * return value of specified cookie
 */
bool cgi_ck( const vector<HTTPCookie> & cks, const string & ck, HTTPCookie & v ) {
    vector<HTTPCookie>::const_iterator ptr = cks.begin(), end = cks.end();
    for( ; ptr!=end; ptr++ ) {
            if( stringsAreEqual(ptr->getName(), ck) ) {
                    v = *ptr;
                    return true;
            }
    }
    return false;
}
/**
 * \return NULL if cookie not found
 */
HTTPCookie *cgi_ck(vector<HTTPCookie> & cks, const string & ck ) throw () {
    vector<HTTPCookie>::iterator ptr = cks.begin(), end = cks.end();
    for( ; ptr!=end; ptr++ )
            if( stringsAreEqual(ptr->getName(), ck) )
                    return &*ptr;
    return NULL;
}

/**
 * add cookies to dest
 */
void cookies_cp( vector<HTTPCookie> & dest, const vector<HTTPCookie> &src ) 
    throw () {
    dest.reserve(dest.size()+src.size()+15);
    vector<HTTPCookie>::const_iterator ptr = src.begin(), end = src.end();
    for( ; ptr!=end; ++ptr )
            dest.push_back(*ptr);
}
