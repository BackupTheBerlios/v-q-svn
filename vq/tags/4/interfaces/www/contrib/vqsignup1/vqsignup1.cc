/**
Copyright (c) 2003 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#include <iostream>
#include <string>
#include <stdexcept>
#include <cgicc/Cgicc.h>
#include <deque>
#include <fstream>
#include <cerrno>
#include <algorithm>
#include <unistd.h>

#define HAVE_NAMESPACE_STD 1
#include <pqxx/all.h>

#include <cvq.h>
#include <util.h>
#include <sig.h>
#include <valid.h>

#include "cgi.h"
#include "vqsignup1_conf.h"
#include "html.h"
#include "lower.h"
#include "sabmain.h"

using namespace std;
using namespace cgicc;
using namespace vqsignup1;
using namespace valid;

Cgicc *cgi=NULL;
const_form_iterator cgie;
const CgiEnvironment *ce = NULL;
const char *me= NULL;

ostream *o=NULL, *e=NULL;
string user, pass, pass1, domain, f_name, f_nip, f_country, f_city, f_str, f_house_no;
string f_app_no, f_pcode, c_name, c_email, c_phone, f_area;
deque<string> domains;
const char *iid=NULL; // id from users_info
char *pl_areas[] = { 
        "0\0wybierz województwo",
        "1\0dolnośląskie",
        "2\0kujawsko-pomorskie",
        "3\0lubelskie",
        "4\0lubuskie",
        "5\0łódzkie",
        "6\0małopolskie",
        "7\0mazowieckie",
        "8\0opolskie",
        "9\0podkarpackie",
        "a\0podlaskie",
        "b\0pomorskie",
        "c\0śląskie",
        "d\0świętokrzyskie",
        "e\0warmińsko-mazurskie",
        "f\0wielkopolskie",
        "A\0zachodniopomorskie"
};

/**
 *
 */
void get_domains( deque<string> & domains ) {
    ifstream in(ac_domains.val_str().c_str());
    if(!in) 
			throw runtime_error((string)"can't open: "+ac_domains.val_str()
				+": "+strerror(errno));
    string ln;
    while(getline(in, ln)) {
            domains.push_back(ln);
    }
    if(in.bad()) 
            throw runtime_error((string)"error while reading: "+ac_domains.val_str()
				+": "+strerror(errno));
}

/**
 */
void notify_run() {
	string notify(ac_notify.val_str());
    char *av[] = { 
            (char *)notify.c_str(), 
            (char *)domain.c_str(), 
            (char *)user.c_str(), (char *)iid, NULL };
    switch(vfork()) {
    case -1:
      *e<<me<<": vfork failed: "<<strerror(errno)<<endl;
      break;
    case 0:
      execvp( *av, av );
      *e<<me<<": execvp: "<<notify<<": "<<strerror(errno)<<endl;
      _exit(1);
    }
}
/**
*/
bool post_act() {
#define ok_not(x) do { \
        *o<<x; \
        ok = false; \
} while(0)
    bool ok = true, pol=false;
    string::size_type len;

    if( stringsAreEqual(f_country, "polska") ) pol = true;
    
    *o<<"<inv><user ";
    len = user.length();
    if(len<3) ok_not("sh=\"1\" ");
    else if(len>30) ok_not("lo=\"1\" ");
    
    if(htmlbad(user, ac_user_ok.val_str().c_str(), ac_html_br.val_str().at(0))) 
            ok_not("dirty=\"1\" ");
    *o<<"/>";
    if(find(domains.begin(), domains.end(), domain)==domains.end()) {
            domain = domains[0];
            ok_not("<domain/>");
    }
    *o<<"<pass ";
    len = pass.length();
    if(len<3) ok_not("sh=\"1\" ");
    else if(len>15) ok_not("lo=\"1\" ");
    if(htmlbad(pass, ac_pass_ok.val_str().c_str(), ac_html_br.val_str().at(0)))
            ok_not("dirty=\"1\" ");
    if( pass != pass1 ) ok_not(" nomatch=\"1\" ");
    *o<<"/>";
    // firm name
    len = f_name.length();
    *o<<"<f_name ";
    if(len < 5) ok_not("sh=\"1\"");
    *o<<"/>";
    // NIP
    *o<<"<f_nip ";
    if( pol && !pl_nip_valid(f_nip).empty())
        ok_not("inv=\"1\" ");
    *o<<"/>"; 
    // country
    *o<<"<f_country ";
    len = f_country.length();
    if(len <= 3) ok_not("sh=\"1\" ");
    *o<<"/>";
    // area
    unsigned i, s;
    for( i=0, s=sizeof(pl_areas)/sizeof(*pl_areas); i<s; ++i )
            if( f_area == pl_areas[i] ) break;
    *o<<"<area ";
    if( i == s ) {
            f_area = pl_areas[0];
            ok_not("inv=\"1\"");
    } else if(f_area == pl_areas[0]) ok_not("no=\"1\"");
    *o<<"/>";
    // city
    *o<<"<f_city ";
    len = f_city.length();
    if(len<=2) ok_not("sh=\"1\" ");
    *o<<"/>";
    // street
    *o<<"<f_str ";
    len = f_str.length();
    if(len<=2) ok_not("sh=\"1\" ");
    *o<<"/>";
    // house no.
    *o<<"<f_house_no ";
    if(f_house_no.empty()) ok_not("sh=\"1\" ");
    else if( htmlbad(f_house_no, 
            "0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM", '_') ) 
            ok_not("dirty=\"1\" ");
    *o<<"/>";
    // appartement no.
    *o<<"<f_app_no ";
    if( ! f_app_no.empty() && htmlbad(f_app_no, "0987654321", '_'))
            ok_not("dirty=\"1\" "); 
    *o<<"/>"; 
    // post code
    *o<<"<f_pcode ";
    if( pol && ! pl_pcode_valid(f_pcode).empty() ) ok_not("inv=\"1\" ");
    *o<<"/>";
    // contact: name
    *o<<"<c_name ";
    len = c_name.length();
    if(len < 5) ok_not("sh=\"1\" ");
    *o<<"/>";
    // contact: other e-mail
    *o<<"<c_email ";
    if(c_email.empty()) ok_not("empty=\"1\" ");
    else if( !email_valid(c_email).empty() ) ok_not("inv=\"1\" ");
    *o<<"/>";
    // contact: phone no.
    *o<<"<c_phone ";
    if( ! c_phone.empty() ) {
            if( c_phone.length() < 6 ) ok_not("sh=\"1\" ");
            else if( htmlbad(c_phone, "0123456789-() +", '_') ) ok_not("dirty=\"1\"");
    }
    *o<<"/>";
    
    *o<<"</inv>";
    if( ok ) {
            cvq *vq;
            int ret;
			pqxx::Connection pg(ac_pgsql.val_str().c_str());

			try {
					pqxx::Transaction tran(pg);
					pqxx::Result res(tran.Exec(
						"SELECT NEXTVAL('users_info_id_seq')") );

					if( res.size() == 1 && (iid = res[0][0].c_str())) {
							tran.Exec( "INSERT INTO users_info "
								"(id,login,domain_id,"
								"f_name,f_nip,f_country,f_area,f_city,"
								"f_str,f_house_no,f_app_no,f_pcode,"
								"c_name,c_email,c_phone) VALUES("
								+pqxx::Quote(iid, false)+","
								+pqxx::Quote(user, false)+","
								"(SELECT ID FROM DOMAINS WHERE DOMAIN="
								+pqxx::Quote(domain, false)+" LIMIT 1),"
								+pqxx::Quote(f_name, false)+","
								+pqxx::Quote(f_nip, false)+","
								+pqxx::Quote(f_country, false)+","
								+pqxx::Quote(f_area, false)+","
								+pqxx::Quote(f_city, false)+","
								+pqxx::Quote(f_str, false)+","
								+pqxx::Quote(f_house_no, false)+","
								+pqxx::Quote(f_app_no, false)+","
								+pqxx::Quote(f_pcode, false)+","
								+pqxx::Quote(c_name, false)+","
								+pqxx::Quote(c_email, false)+","
								+pqxx::Quote(c_phone, false)+")");

							vq = cvq::alloc();
							if( (ret=vq->user_add(user, domain, pass)) ) {
									*e<<me<<": vq->user_add: "<<vq->err_report()<<endl;
							} else {
									*o<<"<add ok=\"\"/>";
									if(! ac_notify.val_str().empty()) 
											notify_run();
									return true;
							}
                    } else 
							*e<<me<<"select nextval"<<endl;
			} catch( const exception & ex ) {
					*e<<me<<": "<<ex.what()<<endl;
			}
            *o<<"<add user_ex=\"\"/>";
    }
    return false;
#undef ok_not
}
/**
 *
 */
void start( int ac, char **av, ostream & o, ostream & e, xslt_args_type & )
{
    // setup
    me = *av;
    ::o = &o;
    ::e = &e;
   
    if(!sig_chld_nocldwait()) throw runtime_error("can't setup signal handler for SIGCHLD");
            
    cgi = new Cgicc();
    cgie = cgi->getElements().end();
    ce = & cgi->getEnvironment();
    
    // real action
    cgi_var(cgi, "user", user); user=lower(user);
    cgi_var(cgi, "domain", domain ); domain=lower(domain);
    cgi_var(cgi, "pass", pass);
    cgi_var(cgi, "pass1", pass1);
    cgi_var(cgi, "f_name", f_name);
    cgi_var(cgi, "f_nip", f_nip);
    cgi_var(cgi, "f_country", f_country);
    cgi_var(cgi, "f_area", f_area); f_area = f_area[0];
    cgi_var(cgi, "f_city", f_city);
    cgi_var(cgi, "f_str", f_str);
    cgi_var(cgi, "f_house_no", f_house_no);
    cgi_var(cgi, "f_app_no", f_app_no);
    cgi_var(cgi, "f_pcode", f_pcode);
    cgi_var(cgi, "c_name", c_name);
    cgi_var(cgi, "c_email", c_email);
    cgi_var(cgi, "c_phone", c_phone);
#define spec_dec(x) x = html_spec_dec(x)
    spec_dec(f_name);
    spec_dec(f_nip);
    spec_dec(f_country);
    spec_dec(f_city);
    spec_dec(f_str);
    spec_dec(f_house_no);
    spec_dec(f_app_no);
    spec_dec(f_pcode);
    spec_dec(c_name);
    spec_dec(c_email);
    spec_dec(c_phone);
#undef spec_dec

    get_domains(domains);

    o<<"<vqsignup>";
    if( stringsAreEqual(ce->getRequestMethod(),"POST") ) {
            if( post_act() ) goto start_end;
    }
    o<<"<domains>";
    for(unsigned i=0, s = domains.size(); i<s; ++i ) {
            o<<"<domain val=\"" << domains[i] << "\"";
            if( domain==domains[i] ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</domains>";
    o<<"<areas>";
    for(unsigned i=0, s=sizeof(pl_areas)/sizeof(*pl_areas); i<s; ++i ) {
            o<<"<area id=\""<< pl_areas[i] 
             <<"\" name=\""<< pl_areas[i]+2 <<"\"";
            if( f_area == pl_areas[i] ) o<<" sel=\"1\" ";
            o<<"/>";
    }
    o<<"</areas>";
    o<<"<user val=\"" << user << "\"/>";
    o<<"<pass val=\"" << pass << "\"/>";
    o<<"<pass1 val=\"" << pass1 << "\"/>";
    
    if(f_country.empty()) f_country = "Polska";
    if(f_name.empty()) f_name = "Polska TEst";
    if(f_nip.empty()) f_nip = "6692324169";
    if(f_city.empty()) f_city = "Kolin";
    if(f_str.empty()) f_str = "Kraxa";
    if(f_house_no.empty()) f_house_no = "23a";
    if(f_pcode.empty()) f_pcode = "23-322";
    if(c_name.empty()) c_name = "bu bu";
    if(c_phone.empty()) c_phone = "(234-234) +342";
    if(c_email.empty()) c_email = "Polska@zeto.pl";
#define spec_enc(x) o<<"<" #x " val=\"" << html_spec_enc(x) << "\"/>"
    spec_enc(f_name);
    spec_enc(f_nip);
    spec_enc(f_country);
    spec_enc(f_city);
    spec_enc(f_str);
    spec_enc(f_house_no);
    spec_enc(f_app_no);
    spec_enc(f_pcode);
    spec_enc(c_name);
    spec_enc(c_email);
    spec_enc(c_phone);
#undef spec_enc

start_end:
    o<<"</vqsignup>";
}
