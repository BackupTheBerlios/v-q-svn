#include <iostream>
#include <new>
#include <string>
#include <stdexcept>
#include <cgicc/Cgicc.h>
#include <deque>
#include <fstream>
#include <errno.h>
#include <algorithm>

#include "cgi.h"
#include "conf_domains.h"
#include "conf_pass_ok.h"
#include "conf_user_ok.h"
#include "conf_html_br.h"
#include "html.h"
#include "vq_init.h"

using namespace std;
using namespace cgicc;

Cgicc *cgi=NULL;
const_form_iterator cgie;
const CgiEnvironment *ce = NULL;

ostream *o=NULL, *e=NULL;

/**
 *
 */
void get_domains( deque<string> & domains ) {
    ifstream in(conf_domains.c_str(), ios::nocreate);
    if(!in) throw runtime_error((string)"can't open: "+conf_domains+": "+strerror(errno));
    string ln;
    while(getline(in, ln)) {
            domains.push_back(ln);
    }
    if(in.bad()) 
            throw runtime_error((string)"error while reading: "+conf_domains+": "+strerror(errno));
}

/**
 *
 */
void start( int ac, char **av, ostream & o, ostream & e )
{
    ::o = &o;
    ::e = &e;
    cgi = new Cgicc();
    cgie = cgi->getElements().end();
    ce = & cgi->getEnvironment();
    string user, pass, pass1, domain;
    deque<string> domains;
    cgi_var(cgi, "user", user);
    cgi_var(cgi, "domain", domain );
    cgi_var(cgi, "pass", pass);
    cgi_var(cgi, "pass1", pass1);
    get_domains(domains);

    o<<"<vqsignup>";
    if( stringsAreEqual(ce->getRequestMethod(),"POST") ) {
            bool ok = true;
            string::size_type len;
            o<<"<inv><user ";
            len = user.length();
            if(len<3) {
                    o<<"tooshort=\"1\" ";
                    ok = false;
            } else if(len>30) {
                    o<<"toolong=\"1\" ";
                    ok = false;
            } 
            if(htmlbad(user, conf_user_ok.c_str(), conf_html_br[0])) {
                    o<<"dirty=\"1\" ";
                    ok = false;
            }
            o<<"/>";
            if(find(domains.begin(), domains.end(), domain)==domains.end()) {
                    ok = false;
                    domain = domains[0];
                    o<<"<domain/>";
            }
            o<<"<pass ";
            len = pass.length();
            if(len<3) {
                    o<<"tooshort=\"1\" ";
                    ok = false;
            } else if(len>15) {
                    o<<"toolong=\"1\" ";
                    ok = false;
            } 
            if(htmlbad(pass, conf_pass_ok.c_str(), conf_html_br[0])) {
                    o<<"dirty=\"1\" ";
                    ok = false;
            }
            if( pass != pass1 ) {
                    o<<" nomatch=\"1\" ";
            }
            o<<"/>";
            o<<"</inv>";
            if( ok ) {
                    cvq *vq;
                    cauth *auth;
                    int ret;
                    vq_init(&vq,&auth);
                    if( (ret=vq->user_add(user, domain, pass)) ) {
                            o<<"<add user_ex=\"\"/>";
                            e<<*av<<": vq->user_add: "<<vq->err_str(ret)<<": "<<vq->err()<<endl;
                    } else {
                            o<<"<add ok=\"\"/></vqsignup>";
                            return;
                    }
            }
    }
    o<<"<domains>";
    for(unsigned i=0, s = domains.size(); i<s; ++i ) {
            o<<"<domain val=\"" << domains[i] << "\"";
            if( domain==domains[i] ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</domains>";
    o<<"<user val=\"" << user << "\"/>";
    o<<"<pass val=\"" << pass << "\"/>";
    o<<"<pass1 val=\"" << pass1 << "\"/>";
    o<<"</vqsignup>";
}
