#include <sys/time.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <exception>
#include <stdexcept>

#include <csess.h>
#include <cgicc/Cgicc.h>

using namespace cgicc;
using namespace std;

/// cgi
Cgicc *cgi=NULL;
/// cgie means not found in cgi
const_form_iterator cgie;
/// environment, all cgi variables
const CgiEnvironment *ce=NULL;
/// session
csess *sess = NULL;

int conf_sess_timeout_i = 20*60;
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
 *
 */
void denied() {
    cout<<"Location: /log/denied.html"<<endl
        <<"Content-Type: text/html"<<endl<<endl;
    exit(0);
}

/**
 *
 */
void empty() {
    cout<<"Location: /log/empty.html"<<endl
        <<"Content-Type: text/html"<<endl<<endl;
    exit(0);
}
/**
 *
 */
bool dom_allow(const string &doms, const string & dom) {
    string::size_type pos, domsl = doms.length(), doml = dom.length();
    string::size_type beg = 0;
    if( doml > domsl ) return false;
    for( ; (pos = doms.find(dom, beg)) != string::npos; beg += (pos-beg)+doml ) {
        if( pos+doml > domsl ) return false;
        if( ! pos ) {
                if( doml == domsl ) return true; // match
                if( doms[pos+doml] == ',' ) return true;
        } else {
                if( doms[pos-1] == ',' ) {
                        if( pos+doml == domsl ) return true;
                        if( pos+doml <= domsl && doms[pos+doml] == ',' ) 
                                return true;
                }
        }
    }
    return false;
}
/**
 *
 */
int main(int , char **av) {
    try {
            string cip; // client ip
            string uri, sid, domains, domain;
            string::size_type uril;
            struct timeval lasttime, now, tvsub;
            
            sess_init(&sess);
            cgi = new Cgicc();
            cgie = cgi->getElements().end();
            ce = &cgi->getEnvironment();
            
            if( ! cgi_var(cgi, "file", uri) || ! cgi_var(cgi, "sid", sid) 
                || ! cgi_var(cgi, "domain", domain) || domain.empty()
                || uri.empty() || sid.empty() ) {
                    denied();
            }
            if( (uril=uri.length()) > 2 ) {
                    if( ( uri[0] == '.' && uri[1] == '.' && uri[2] == '/' )
                       || ( uri[uril-1] == '/' && uri[uril-2] == '.' && uri[uril-3] == '.')
                       || ( uri[uril-1] == '.' && uri[uril-2] == '.' ) 
                       || uri.find("/../") != string::npos ) {
                            denied();
                    }
            }
            
            if( ! sess->open(sid) ) {
                    denied();
            }
            // check if specified session is valid
            if( ! sess->getval("cip", cip) || cip != ce->getRemoteAddr() ) {
                    denied();
            }
            if( gettimeofday( &now, NULL ) ) throw runtime_error("gettimeofday failed!");
            if( sess->getval("lasttime", lasttime) ) {
                    timersub(&now, &lasttime, &tvsub);
                    if( tvsub.tv_sec > conf_sess_timeout_i ) {
                            denied();
                    }
            } else {
                    denied();
            }
            if( ! sess->getval("stat_domains", domains) 
                || ! dom_allow(domains, domain) ) {
                    denied();
            }

            ifstream in(uri.c_str());
            if( ! in ) {
                    empty();
            }
            if( uri[uri.length()-1] == 'g' ) 
                    cout<<"Content-Type: application/png\n\n";
            else 
                    cout<<"Content-Type: text/html\n\n";
            cout<<in.rdbuf();
    } catch( const exception & e ) {
            cerr<<*av<<": exception: "<<e.what()<<endl;
            return 1;
    }
    return 0;
}
