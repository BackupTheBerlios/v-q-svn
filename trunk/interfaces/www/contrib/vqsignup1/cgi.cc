#include "cgicc/Cgicc.h"
#include "cgi.h"

using namespace std;
using namespace cgicc;

bool cgi_var( Cgicc *cgi, const string &name, string &val ) {
    const_form_iterator fi = cgi->getElement(name);
    if( fi != cgi->getElements().end() ) {
            val = fi->getValue();
            return true;
    }
    val = "";
    return false;
}

bool cgi_ck( const vector<HTTPCookie> * cks, const string & ck, string & v ) {
    vector<HTTPCookie>::const_iterator ptr = cks->begin(), end = cks->end();
    for( ; ptr!=end; ptr++ ) {
            if( stringsAreEqual(ptr->getName(), ck) ) {
                    v = ptr->getValue();
                    return true;
            }
    }
    v = "";
    return false;
}
