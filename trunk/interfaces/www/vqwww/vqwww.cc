/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <deque>
#include <ctime>
#include <sys/time.h>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <new>

#include <cvq.h>

#include "vqwww.h"
#include "sabmain.h"
#include "cconfmod.h"
#include "cpassmod.h"
#include "coutmod.h"
#include "credirmod.h"
#include "cautorespmod.h"
#include "cloghistmod.h"
#include "csess.h"
#include "cgi.h"
#include "conf_home.h"
#include "vqwww_conf.h"
#include "html.h"

#ifndef RTLD_NOW
    #define RTLD_NOW 0
#endif

#ifndef RTLD_GLOBAL
    #define RTLD_GLOBAL 0
#endif

using namespace std;
using namespace cgicc;
using namespace vqwww;

/**
 *
 */
cvqwww_dirs::cvqwww_dirs(const std::string &home)
		: conf(home+"/etc/vqwww"), 
		share(home+"/share/vqwww"),
		libexec(home+"/libexec/vqwww"), 
		include(home+"/include/vqwww") {
}

/**
 *
 */
class cvqwww : public cenvironment {
		protected:
				void mods_init();
				void mod_load( const string & file, const string & name );
				void menu_dump();
				void req_proc();
				void cks_dump();
				void sess_crt( const cvq::auth_info & );
				bool sess_get();
				bool log_do_it(bool sto = false);
				void lasttime_now();
				bool log_do();
				void setup();
				void locale_setup( bool = false );
				void locales_dump( ostream & );

				void cgi_rm_set( request_method ); 
		public:
				cvqwww(int ac, const char ** av, ostream & out, ostream & err,
						xslt_args_type & xat )
						: cenvironment(ac, av, out, err, xat) {

					logger->service_set(cgi_env_get().usingHTTPS() ? 
						clogger::ser_https : clogger::ser_http );
					logger->ip_set(cgi_env_get().getRemoteAddr());
				}

				void run();
};

/**
 *
 */
void cvqwww::cgi_rm_set( request_method rm ) {
	cgi_rm = rm;	
}

/**
 * dlopen module, load structure with specified name,
 * call mod_init from structure
 * \throw runtime_error if it can't load module
 */
void cvqwww::mod_load( const string & file, const string & name) {
	// real path
    string rpath( file[0] == '/' ? file : dirs.libexec+'/'+file);
	
    void *dl = dlopen(rpath.c_str(), RTLD_NOW | RTLD_GLOBAL );
    if( ! dl ) 
			throw runtime_error("mod_load: "+rpath+": "+dlerror());
	mod_info *mi = (mod_info *) dlsym(dl, name.c_str());
    if( ! mi ) 
			throw runtime_error("mod_load: symbol: "+name+": "+dlerror());
	
    (*mi).mods_init(mods, *this);
}

/**
 * \brief initializes modules table, it will dlopen some of them in the future
 */
void cvqwww::mods_init() {
    // internal modules
    mods.push_back( new coutmod(*this) );
    mods.push_back( new cconfmod(*this) );
    mods.push_back( new cpassmod(*this) );
    mods.push_back( new credirmod(*this) );
    mods.push_back( new cautorespmod(*this) );
	mods.push_back( new cloghistmod(*this) );

    // external modules, dlopen them
    string fnmods(dirs.conf+"/mods");
    ifstream fmods( fnmods.c_str() );
    if( fmods ) {
            string ln;
            string::size_type sep;
            while( getline(fmods, ln) ) {
                    if( ln[0] == '#' ) continue;
                    sep = ln.find('|');
                    if( sep != string::npos ) {
                    		mod_load(ln.substr(0, sep), ln.substr(sep+1));
                    } else
							err<<*av<<": mods file include invalid line: "
								<<ln<<"; ignoring"<<endl;
            }
            if( ! fmods.bad() ) 
					return;
    } else if( errno == ENOENT )
			return;

	throw runtime_error((string)"error while reading mods file: "+strerror(errno));
} 
/**
 * \brief dump menu
 */
void cvqwww::menu_dump() {
    deque< cmod * >::size_type cnt = mods.size();
    if( cnt -- ) {
        out << "<menu>";
        do {
                if( ! (mods[cnt]->modinfo & cmod::mi_menu) ) continue;
                out <<"<mod id=\"" << mods[cnt]->id() << "\">";
                out << mods[cnt]->menu();
                out << "</mod>";
        } while( cnt-- );
        out << "</menu>";
    }
}
/**
 * \brief process requests
 */
void cvqwww::req_proc() {
    deque<cmod *>::size_type mod_cur, mods_cnt = mods.size();
    if(!mods_cnt) return;
    char res;
    bool at; // does some action was taken?
    for(;;) {
            at = false;
            for(mod_cur=0; mod_cur<mods_cnt; ++mod_cur) {
					try {
							mod_out.clear();
							mod_out.str("");
							res = mods[mod_cur]->run();
					} catch( const std::exception & e ) {
							mod_out.str("");
							out<<"<exception>"<<e.what()<<"</exception>";
							err << *av << ": exception cougth: " << e.what() << endl;
							return;
					} catch( ... ) {
							mod_out.str("");
							out<<"<exception unexpected=\"1\"/>";
							err << *av << ": unknown exception" << endl;
							return;
					}

					out<<mod_out.str();
					if(out.bad() || mod_out.bad()) 
							throw runtime_error("can't copy module's output");
					mod_out.str("");

                    switch(res) {
                    case cmod::done:
                            return;
                    case cmod::loopaf:
                            at= true;
                            break;
                    case cmod::loopa:
                            goto req_proc_le;
                    case cmod::sessclose:
                            sess->rm();
                            out<<"<redirme/>";
                            return;
                    }
            }
            if(!at) break;
req_proc_le:
            continue;
    }
}

/**
 * dump cookies
 */
void cvqwww::cks_dump() {
	cookies_map::const_iterator cur = cgi_cks_set.begin(), end=cgi_cks_set.end();
    for( ; cur!=end; ++cur ) {
            out << "<cookie>";
            cur->second.render(out);
            out << "</cookie>";
    }
}

/**
 * \throw runtime_error If it can't create or set session's value
 */
void cvqwww::sess_crt( const cvq::auth_info & ai ) {
    string sid;
    if( !sess->create(sid) )
            throw runtime_error((string)"can't create session: "+sess->errmsg());

	if( !sess->setval("cip", cgi_env_get().getRemoteAddr())
		|| ! sess->setval("user", ai.user)
		|| ! sess->setval("domain", ai.dom)
		|| ! sess->setval("flags", (char*) &ai.flags, sizeof(ai.flags))
		|| ! sess->setval("dir", ai.dir) )
			throw runtime_error((string)"can't set session's variable: "+sess->errmsg());

    HTTPCookie ck_sid;
    ck_sid.setName("SID"); 
    ck_sid.setValue(sid);
    cgi_cks_set[ck_sid.getName()] = ck_sid;
}            

/**
 * read in variables from session
 * \return false on error
 */
bool cvqwww::sess_get() {
    if( ! sess->getval("user", sessb.ai.user)
       || ! sess->getval("domain", sessb.ai.dom)
       || ! sess->getval("flags", (char*) &sessb.ai.flags, sizeof(sessb.ai.flags))
       || ! sess->getval("dir", sessb.ai.dir) ) {
            return false;
    }
    return true;
}

/**
 * Prints supported locales
 */
void cvqwww::locales_dump( ostream & os ) {
	const cmapconf::map_type & locales = ac_locales.val_map();
	
	cmapconf::map_type::const_iterator beg, end;
	for( beg = locales.begin(), end = locales.end(); beg != end; ++beg ) {
			os<<"<locale code=\""<< beg->first 
				<< "\" name=\"" << beg->second << "\"";
			if( beg->first == locale )
					os << " sel=\"1\" ";
			os<<"/>";
	}
}

/**
 * \param sto - true means session timed out
 */
bool cvqwww::log_do_it(bool sto) {
    string email, pass;
    ostringstream os;

    os<<"<log>";

    if(sto) os<<"<sto/>";

    if( cgi_rm_get() == rm_post && cgi->getElement("login") != cgi_end_get() ) {
    		cvq::auth_info ai;
            bool ok = true;

			locale_setup(true);

            os<<"<inv><email ";
            if(cgi_var(cgi.get(),"email", email) && ! email.empty()) {
                    string::size_type atpos;
                    if(htmlbad(email, ac_user_ok.val_str().c_str(), 
						ac_html_br.val_str().at(0))) {
                            ok = false;
                            os<<" dirty=\"1\" ";
                    } else if((atpos=email.find('@')) == string::npos) {
                            ok = false;
                            os<<" syntax=\"1\" ";
                    } else {
                            ai.user = email.substr(0, atpos );
                            ai.dom = email.substr( atpos+1 );
							logger->domain_set(ai.dom);
							logger->login_set(ai.user);
                    }
            } else { 
					ok=false; 
					os << " empty=\"1\" "; 
			}
            
            os<<"/><pass ";

            if(cgi_var(cgi.get(),"pass", pass) && ! pass.empty() ) {
                    if(htmlbad(pass, ac_pass_ok.val_str().c_str(), 
						ac_html_br.val_str().at(0))) {
                            ok = false;
                            os<<" dirty = \"1\" ";
							logger->write(clogger::re_data, "dirty password: "+pass);
                    }
            } else { 
					ok = false; 
					os<<" empty = \"1\" ";
					logger->write(clogger::re_data, "empty password");
			}
            
            os<<"/>";
            
            // if ok do authorization
            if( ok ) {
                    switch(vq->user_auth(ai) ) {
					case cvq::err_no:
							if( pass == ai.pass ) {
									sess_crt( ai ); // this will throw exception on error
									logger->write(clogger::re_ok, "");
									return sess_get();
							} else 
									logger->write(clogger::re_pass, pass);
							break;
					case cvq::err_user_nf:
							logger->write(clogger::re_data, "no such user");
							break;
					default:
							logger->write(clogger::re_int, vq->err_report());
					}
					os << "<auth/>";
            }
            os<<"</inv>";
    }
	
	out << os.str()
        << "<email val=\"" << email << "\"/>";
	
	locales_dump(out);

	out	<<"</log>";
    return false;
}

/**
 *
 */
void cvqwww::lasttime_now() {
    struct timeval now;
    if(! gettimeofday(&now, NULL)) {
            if( ! sess->setval("lasttime", now) )
                    throw runtime_error("can't set lasttime");
    } else 
			throw runtime_error("gettimeofday != 0");
}

/**
 * \brief do login, at the login time session is created, client ip and
 * login time are saved in it, they are checked every request
 * \return true on success (valid login)
 */
bool cvqwww::log_do() {
    HTTPCookie ck_sid;

    // if session doesn't exist. create it. 
    if( ! cgi_ck(cgi_cks_get(), "SID", ck_sid)
       || ! sess->open(ck_sid.getValue())
	   || ! sess->getval("cip", sessb.cip) 
	   || sessb.cip != cgi_env_get().getRemoteAddr() ) {
            return log_do_it();
    }
    
    struct timeval lasttime;
    if( sess->getval("lasttime", lasttime) ) {
            struct timeval sub, now;
            if(gettimeofday(&now, NULL))
                    throw runtime_error((string)"gettimeofday: "+strerror(errno));

            timersub(&now, &lasttime, &sub);
            if(sub.tv_sec>ac_sess_timeout.val_int()) {
                    return log_do_it(true);
            }

		    if( ! sess_get() ) 
					return log_do_it(true);

		    return true;
    }
	return log_do_it();
}

/**
 * \param login if true try to read POST variable locale first
 */
void cvqwww::locale_setup( bool login ) {
	HTTPCookie ck_loc;
	if( login ) {
			if( ! cgi_var(cgi.get(), "locale", locale) )
					login = false;
			if( locale.empty() )
					login = false;
	}
	
	if( ! login && cgi_ck(cgi_cks_get(), "locale", ck_loc) ) {
			locale = ck_loc.getValue();
	}

	if( ! locale.empty() ) {
			ck_loc.setName("locale");
			ck_loc.setValue(locale);
			ck_loc.setMaxAge(60*60*24*7); // week

			cgi_cks_set[ck_loc.getName()] = ck_loc;
	} else
			locale = "pl_PL";

	xslt_args["LOCALE"] = locale;
}

/**
 * all setup before real action put here
 */
void cvqwww::setup() {
	cgi_rm_set( stringsAreEqual(cgi_env_get().getRequestMethod(), "post") 
		? rm_post : rm_get );

	locale_setup();

	// setup all variables needed for xslt processing 
	xslt_args["REQUEST_URI"] = cgi_env_get().getScriptName();
	xslt_args["SERVER_NAME"] = cgi_env_get().getServerName();
	if( cgi_env_get().usingHTTPS() )
			xslt_args["HTTPS"] = "on";
}

/**
 *
 */
void cvqwww::run() {
    // setup some data
    setup();

    out<<"<vqwww>";
    if( log_do() ) {
            lasttime_now();

            mods_init();
            menu_dump();

            req_proc();
    } 
   
	cks_dump();
    out<<"</vqwww>";
}

/**
 *
 */
void start( int ac, char **av, ostream &out, ostream &err, 
		xslt_args_type & xat ) {
	cvqwww vqwww(ac, (const char **)av, out, err, xat);
	vqwww.run();
}
