#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <deque>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <errno.h>
#include <stdexcept>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <new>

#include "vqwww.h"
#include "sabmain.h"
#include "cconfmod.h"
#include "cpassmod.h"
#include "coutmod.h"
#include "credirmod.h"
#include "csess.h"
#include "cgi.h"
#include "conf_sess_timeout.h"
#include "vq_init.h"
#include "conf_html_br.h"
#include "conf_user_ok.h"
#include "conf_pass_ok.h"
#include "conf_home.h"
#include "html.h"

#ifndef RTLD_NOW
#define RTLD_NOW 0
#endif

#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif

using namespace std;
using namespace cgicc;

/*********************************************************************/
char **av=NULL;
int ac=0;

ostream *o=NULL, *e=NULL; //!< output streams

Cgicc *cgi=NULL; //!< cgi

const_form_iterator cgie; //!< cgie means not found in cgi

const CgiEnvironment *ce=NULL; //!< environment, all cgi variables
/// cookies to set
vector<HTTPCookie> cks_set;
/// currently set cookies
const vector<HTTPCookie> *cks_cur;
/// request method
enum _reqmet reqmet;

/// session
csess *sess=NULL;
/// vq's authorization
cauth *auth=NULL;
/// vq's main class
cvq *vq=NULL;
/// variables stored in session
struct _sessv sessv;
/// 
struct _vqwww_dirs vqwww_dirs = { 
        conf_home, 
        conf_home+"/etc/vqwww", 
        conf_home+"/share/vqwww", 
        conf_home+"/libexec/vqwww",
        conf_home+"/include/vqwww"
};
/// loaded modules
deque< cmod* > mods;

static const char *xsltargs1[] = {
        "uri", NULL,
        NULL
};

/**
 * dlopen module, load structure with specified name,
 * call mod_init from structure
 * \throw runtime_error if it can't load module
 */
void mod_load( const string & file, const string & name ) throw(runtime_error) {
    string rpath; // real path
    if( file[0] == '/' ) rpath = file;
    else rpath = vqwww_dirs.libexec+'/'+file;
    void *dl = dlopen(rpath.c_str(), RTLD_NOW | RTLD_GLOBAL );
    if( ! dl ) throw runtime_error("mod_load: "+rpath+": "+dlerror());
    mod_info *mi = (mod_info *) dlsym(dl, name.c_str());
    if( ! mi ) throw runtime_error("mod_load: symbol: "+name+": "+dlerror());
    (*mi).mods_init(mods);
}

/**********************************************************************
 * \brief initializes modules table, it will dlopen some of them in the future
 **********************************************************************/
void mods_init() throw(runtime_error) {
    // internal modules
    mods.push_back( new coutmod() );
    mods.push_back( new cconfmod() );
    mods.push_back( new cpassmod() );
    mods.push_back( new credirmod() );

    // external modules, dlopen them
    string fnmods = vqwww_dirs.conf+"/mods";
    ifstream fmods( fnmods.c_str(), ios::nocreate );
    if( fmods ) {
            string ln;
            string::size_type sep;
            while( getline(fmods, ln) ) {
                    if( ln[0] == '#' ) continue;
                    sep = ln.find('|');
                    if( sep == string::npos ) {
                            *e<<*av<<": mods file include invalid line: "<<ln<<"; ignoring"<<endl;
                            continue;
                    }
                    mod_load(ln.substr(0, sep), ln.substr(sep+1));
            }
            if( fmods.bad() ) throw runtime_error((string)"error while reading mods file: "+strerror(errno));
    }
} 
/**********************************************************************
 * \brief dump menu
 *  *******************************************************************/
void menu_dump() {
    deque< cmod * >::size_type cnt = mods.size();
    if( cnt -- ) {
        *o << "<menu>";
        do {
                if( ! (mods[cnt]->modinfo & cmod::mi_menu) ) continue;
                *o <<"<mod id=\"" << mods[cnt]->id() << "\">";
                *o << mods[cnt]->menu();
                *o << "</mod>";
        } while( cnt-- );
        *o << "</menu>";
    }
}
/**********************************************************************
 * \brief process requests
 * ********************************************************************/
void req_proc() {
    deque<cmod *>::size_type mod_cur, mods_cnt = mods.size();
    if(!mods_cnt) return;
    char res;
    bool at; // does some action was taken?
    for(;;) {
            at = false;
            for(mod_cur=0; mod_cur<mods_cnt; ++mod_cur) {
                    res = mods[mod_cur]->run();
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
                            *o<<"<redirme/>";
                            return;
                    }
            }
            if(!at) break;
req_proc_le:
            continue;
    }
}
/**********************************************************************
 * dump cookies
 * ********************************************************************/
void cks_dump(ostream *o) {
    const_cookie_iterator cur = cks_set.begin(), end=cks_set.end();
    for( ; cur!=end; cur++ ) {
            *o << "<cookie>";
            cur->render(*o);
            *o << "</cookie>";
    }
}
/**********************************************************************
 *
 * ********************************************************************/
void sess_crt() {
    string sid;
    HTTPCookie ck_sid;

    if( !sess->create(sid) 
        || !sess->setval("cip", ce->getRemoteAddr()))
            throw runtime_error((string)"can't create session: "+sess->errmsg());

    ck_sid.setName("SID"); 
    ck_sid.setValue(sid);
    cks_set.push_back(ck_sid);
}            

/**********************************************************************
 *
 **********************************************************************/
void cgi2sess() {
    try {
            cgi->save((string)sess->dir()+"/cgi");
            sess->setval("saved", true);
    } catch(...) {
            unlink(((string)sess->dir()+"/cgi").c_str());
            sess->rmval("saved");
    }
}
/**
 * read in variables from session
 * \return false on error
 */
bool sess_get() {
    if( ! sess->getval("user", sessv.ai.user)
       || ! sess->getval("domain", sessv.ai.dom)
       || ! sess->getval("flags", (char*) &sessv.ai.flags, sizeof(sessv.ai.flags))
       || ! sess->getval("dir", sessv.ai.dir)) {
            cgi2sess();
            return false;
    }
    return true;
}
/**********************************************************************
 * \param sto - true means session timed out
 **********************************************************************/
bool log_do_it(bool sto = false) {
    string email, pass;
    ostringstream os;
    cvq::auth_info ai;

    cks_dump(&os);
    os<<"<log>";
    if(sto) os<<"<sto/>";
    if( reqmet == post && cgi->getElement("login") != cgie ) {
            bool ok = true;
            os<<"<inv><email ";
            if(cgi_var(cgi,"email", email) && ! email.empty()) {
                    string::size_type atpos;
                    if(htmlbad(email, conf_user_ok.c_str(), conf_html_br[0])) {
                            ok = false;
                            os<<" dirty=\"1\" ";
                    } else if((atpos=email.find('@')) == string::npos) {
                            ok = false;
                            os<<" syntax=\"1\" ";
                    } else {
                            ai.user = email.substr(0, atpos );
                            ai.dom = email.substr( atpos+1 );
                    }
            } else { ok=false; os << " empty=\"1\" "; }
            
            os<<"/><pass ";

            if(cgi_var(cgi,"pass", pass) && ! pass.empty() ) {
                    if(htmlbad(pass, conf_pass_ok.c_str(), conf_html_br[0])) {
                            ok = false;
                            os<<" dirty = \"1\" ";
                    }
            } else { ok = false; os << " empty=\"1\" "; }
            
            os<<"/>";
            
            // if ok do authorization
            if( ok ) {
                    struct timeval now;
                    if(gettimeofday(&now, NULL))
                            throw runtime_error("gettimeofday failed");
                    if(! vq->user_auth(ai)
                       && pass == ai.pass ) {
                            sess_crt();
                            if( sess->setval("user", ai.user)
                                && sess->setval("domain", ai.dom)
                                && sess->setval("flags", 
                                        (char*) &ai.flags, sizeof(ai.flags))
                                && sess->setval("dir", ai.dir)) {
                                    return sess_get();
                            } else os << "<auth/>"; 
                    } else os << "<auth/>";
            }
            os<<"</inv>";
    }
    *o << os.str()
        << "<email val=\"" << email << "\"/></log>";
    return false;
}
/**********************************************************************
 *
 **********************************************************************/
void log_do_ok() {
    struct timeval now;
    if(! gettimeofday(&now, NULL)) {
            if( !sess->setval("lasttime", now) )
                    throw runtime_error("can't set lasttime");
    } else throw runtime_error("gettimeofday != 0");
}
/**********************************************************************
 * \brief do login, at the login time session is created, client ip and
 * login time are saved in it, they are checked every request
 * \return true on success (valid login)
 * ********************************************************************/
bool log_do() {
    struct timeval lasttime, now;
    string ip;
    HTTPCookie ck_sid;

    // if session doesn't exist. create it. 
    if(!cgi_ck(*cks_cur, (string)"SID", ck_sid)
       || !sess->open(ck_sid.getValue()) ) {
            return log_do_it();
    }
    
    if( ! sess->getval("cip", sessv.cip) || sessv.cip != ce->getRemoteAddr() ) {
            return log_do_it();
    }            

    if( sess->getval("lasttime", lasttime) ) {
            struct timeval sub;
            if(gettimeofday(&now, NULL))
                    throw runtime_error((string)"gettimeofday: "+strerror(errno));
            timersub(&now, &lasttime, &sub);
            if(sub.tv_sec>conf_sess_timeout_i) {
                    return log_do_it(true);
            }
    } else {
            return log_do_it();
    }
    
    if( ! sess_get() ) return log_do_it();
    return true;
}
/**********************************************************************
 * all setup before real action put here
 * ********************************************************************/
void setup() {
    cks_cur = &ce->getCookieList();
    string rm = ce->getRequestMethod();
    if( stringsAreEqual(rm, "post") ) reqmet = post;
    else reqmet = get;
}
/**********************************************************************
 *
 **********************************************************************/
void start( int ac, char **av, ostream &out, ostream &err ) {
    ::ac = ac;
    ::av = av;
    o = &out;
    e = &err;
    
    vq_init(&vq, &auth);
    cgi = new Cgicc();
    cgie = cgi->getElements().end();
    sess_init(&sess);
    ce = &cgi->getEnvironment();

    // setup all variables needed for xslt processing 
    xsltargs1[0*2+1] = (new string(ce->getScriptName()))->c_str();
    xsltargs = xsltargs1;
   
    // setup some data
    setup();

    *o<<"<vqwww>";
    if( log_do() ) {
            log_do_ok();
            cks_dump(o);
            // if session is valid show everything, check whether we have stored environment
            mods_init();
            menu_dump();
            req_proc();
    } 
    *o << "</vqwww>";
}
