/**
WWWADS

Copyright (c) 2002 Pawel Niewiadomski
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

@author Pawel Niewiadomski
@version 1
*/

#include <libgen.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>
#include <sstream>
#include <deque>
#include <string>
#include <time.h>

#include <cgicc/Cgicc.h>
#include <sablot.h>

#include "cads.h"
#include "conf_adsdir.h"
#include "conf_wwwdir.h"
#include "conf_htmlok.h"
#include "conf_htmlbadrep.h"
#include "adsd.h"
#include "getline.h"
#include "is_uint32_t.h"
#include "html.h"
#include "fd.h"
#include "fdstr.h"
#include "sig.h"
#include "sabmain.h"

using namespace std;
using namespace cgicc;

/**************************************************************************
 * GLOBAL VARIABLES PUT HERE !
 **************************************************************************/
typedef struct { string v; bool d; } cgi_var;

/// socket
int so=-1;
struct sockaddr_un sun;

/// program's name
char *me = NULL;
/// class for any CGI interaction
Cgicc *cgi = NULL;
/// not found marker
const_form_iterator cgiend;
/// CGI environment
const CgiEnvironment * cgie = NULL;
/// CGI cookies
vector<HTTPCookie> cgico;
/// output streams
ostream *o = NULL, *e = NULL;

///
deque <string> gids, gnames;

const char * xsltargs1[] = { "uri", NULL, "uniq", NULL, NULL };

/**
finds a cookie in cgico
@return cookie's position, cgico.size()+1 if not found
@param c name of cookie
*/
vector<HTTPCookie>::size_type cookiefind( string c )
{
    vector <HTTPCookie>::size_type i = cgico.size();
    if( ! i ) return i+1;
    i--;
    do {
            if( cgico[i].getName() == c ) return i;
    } while( i-- );
    return cgico.size()+1;
}

/**
*/
void cgi_get_var( const string & name, cgi_var & v )
{
    const_form_iterator fi = cgi->getElement(name);
    if( fi != cgiend ) {
            v.v = fi->getValue();
            v.d = htmlbad(v.v, conf_htmlok.c_str(), conf_htmlbadrep[0] );
    } else {
            v.v = "";
            v.d = false;
    }
}
/**
add group form
*/
void grps_add()
{
    string method = cgie->getRequestMethod();
    *o <<"<wwwads><grps><add>";
    
    cgi_var name;
    cgi_get_var("name", name);

    if( stringsAreEqual(method,"POST")) {
            if( ! name.d ) {
                    if( ! name.v.empty() && name.v.length() < 20 ) {
                            char resp;
                            // add group
                            if( fdwrite(so, &ADSD_GADD,sizeof(ADSD_GADD)) == sizeof(ADSD_GADD)
                                && fdwrstr(so,name.v) != -1 
                                && fdread(so,&resp,1) == 1
                                && resp == 'K' ) {
                                    name.v = "";
                                    *o << "<ok/>";
                            } else {
                                    *o << "<cant/>";
                                    *e << me << ": can't add new group: " 
                                       << name.v << ": " 
                                       << strerror(errno) << '\n'; 
                            }
                    } else *o<< "<inv len=\"\"/>";
            } else *o<< "<inv dirty=\"\"/>";
    }
    *o <<"<name>" << name.v
       <<"</name></add></grps></wwwads>";
}

/**
show groups
*/
void grps_ls()
{
    *o << "<wwwads><grps><ls>";
    char resp, re=0;
    string grps,name,id;
    deque<string>::size_type cnt;
    if( fdwrite(so,&ADSD_GLS,sizeof(ADSD_GLS)) == sizeof(ADSD_GLS)
        && fdread(so,&resp,sizeof(resp)) == sizeof(resp)
        && resp == 'F' 
        && fdread(so,&cnt,sizeof(cnt)) == sizeof(cnt)) {
            if(cnt--) {
                    do {
                        if( fdrdstr(so,name) != -1
                            && fdrdstr(so,id) ) {
                                grps+="<grp><name>"+name+"</name>";
                                grps+="<id>"+id+"</id></grp>";
                        } else {
                                re = 1;
                                break;
                        }
                    } while(cnt--);
                    if(!re) *o << grps;
            } else *o << "<no/>";
    }
    if(re) {
            *o << "<cant/>";
            *e << me << ": can't get groups list: " 
               << strerror(errno) << '\n'; 
    }
    *o << "</ls></grps></wwwads>";
}

/**
*/
void grps_rm()
{
    const_form_iterator fi1 = cgi->getElement("val1");
    const_form_iterator fi = cgi->getElement("val");
    if( fi1 != cgiend ) {
            char resp;
            if( fdwrite(so, &ADSD_GRM1, sizeof(ADSD_GRM1)) != sizeof(ADSD_GRM1)
                || fdwrstr(so, fi->getValue()) == -1
                || fdread(so, &resp, sizeof(resp)) != sizeof(resp)
                || resp != 'K' )
                    *o <<"<wwwads><grps><rm><cant/></rm></grps></wwwads>";
            else *o <<"<http><redirme>what=1&amp;act=1</redirme></http>";
    } else {
            *o <<"<wwwads><grps><rm>" << fi->getValue()
               <<"</rm></grps></wwwads>";
    }
}

/**
*/
void grps_chg()
{
}

/**
groups processing
*/
void grps_start()
{
    const_form_iterator fi = cgi->getElement("act");
    if( fi != cgiend ) {
            string fiv = fi->getValue();
            if( fiv == "1" ) {
                    fi = cgi->getElement("op");
                    if( fi != cgiend && cgi->getElement("val") != cgiend ) {
                            fiv = fi->getValue();
                            if( fiv == "1" ) {
                                    grps_rm();
                                    return;
                            } else if( fiv == "2" ) {
                                    grps_chg();
                                    return;
                            }
                    }
                    grps_ls();
            } else {
                    grps_add();
            }
    }
}

/**
retrieves groups, changes gids, gnames
@return true on success
*/
bool grps_get()
{
    char resp;
    string name, id;
    cads::size_type cnt;

    if( fdwrite(so,&ADSD_GLS,sizeof(ADSD_GLS)) == sizeof(ADSD_GLS)
        && fdread(so,&resp,sizeof(resp)) == sizeof(resp) 
        && resp == 'F'
        && fdread(so,&cnt,sizeof(cnt)) == sizeof(cnt)) {
        if(cnt--) {
                do {
                    if( fdrdstr(so,name) != -1
                        && fdrdstr(so,id) ) {
                            gnames.push_back(name);
                            gids.push_back(id);
                    } else return false;
                } while(cnt--);
        }
        return true;
    }
    return false;
}

void ads_ls_get( cads::size_type cnt )
{
    cads::ad_info ai;
    time_t ct = time(NULL);
    for( cads::size_type i=0; i < cnt; i ++ ) {
            if( fdrdstr(so, ai.aid) == -1
                || fdrdstr(so, ai.misc) == -1
                || fdrdstr(so, ai.url) == -1
                || fdrdstr(so, ai.asisurl ) == -1
                || fdread(so, &ai.mc, sizeof(ai.mc)) == -1 
                || fdread(so, &ai.cc, sizeof(ai.cc)) == -1
                || fdread(so, &ai.ms, sizeof(ai.ms)) == -1
                || fdread(so, &ai.cs, sizeof(ai.cs)) == -1
                || fdread(so, &ai.ttl, sizeof(ai.ttl)) == -1
                || fdread(so, &ai.blk, sizeof(ai.blk)) == -1 ) {
                    *e << "can't read ad.'s info: " << strerror(errno) << endl;
                    *o << "<cant/>";
                    break;
             }
             *o << "<ad><id>" << ai.aid << "</id>"
                << "<asisurl>" << ai.asisurl << "</asisurl>"
                << "<mc>" << ai.mc << "</mc>"
                << "<cc>" << ai.cc << "</cc>"
                << "<ms>" << ai.ms << "</ms>"
                << "<cs>" << ai.cs << "</cs>"
                << "<misc>" << ai.misc << "</misc>"
                << "<url>" << ai.url << "</url>"
                << "<ttl>" << (ai.ttl-ct)/(3600*24) << "</ttl>";
             if( ai.blk ) *o << "<blk/>";   
             *o <<"</ad>";
    }
}

/**
*/
void ads_ls()
{
    *o << "<wwwads><ads><ls>";
    cgi_var grp;
    cgi_get_var("grp",grp);
    bool grp_ok = false;
    if( grps_get() ) {
            deque<string>::size_type cnt = gnames.size();
            if(cnt--) {
                    *o << "<grps>";
                    do {
                            *o << "<grp><name>" << gnames[cnt]
                              << "</name><id>" << gids[cnt] 
                              << "</id>";
                            if( ! grp.d && grp.v == gids[cnt] ) {
                                    *o << "<selected/>";
                                    grp_ok = true;
                            }
                            *o << "</grp>";
                    } while(cnt--);
                    *o << "</grps>";
            }
            if( grp_ok ) {
                    char resp;
                    deque<cads::ad_info>::size_type ais_cnt;
                    
                    if( fdwrite(so,&ADSD_ALS,sizeof(ADSD_ALS)) == sizeof(ADSD_ALS)
                        && fdwrstr(so,grp.v) != -1
                        && fdread(so,&resp,1) == 1
                        && resp == 'F' 
                        && fdread(so,&ais_cnt,sizeof(ais_cnt)) == sizeof(ais_cnt) ) {
                            if( cnt ) {
                                    *o << "<grp>" << grp.v << "</grp>";
                                    ads_ls_get(ais_cnt);
                            } else *o << "<no/>";
                    } else {
                            *o << "<cant/>";
                            *e << "can't get ads. info: grp=" << grp.v << ": " << strerror(errno) << endl;
                    }
            }
    } else *o << "<cant/>"; 
    *o << "</ls></ads></wwwads>";
}

/**
*/
void ads_rm()
{
    cgi_var val, grp;
    cgi_get_var("val", val);
    cgi_get_var("grp", grp);
    const_form_iterator fi1 = cgi->getElement("val1");
    
    if( fi1 != cgiend ) {
            char resp;
            if( fdwrite(so, &ADSD_ARM1, sizeof(ADSD_ARM1)) != sizeof(ADSD_ARM1)
                || fdwrstr(so, grp.v) == -1
                || fdwrstr(so, val.v) == -1
                || fdread(so, &resp, sizeof(resp)) != sizeof(resp)
                || resp != 'K' )
                    *o<<"<wwwads><ads><rm><cant/></rm></ads></wwwads>";
            else *o<<"<http><redirme>what=2&amp;act=1&amp;grp="<<grp.v<<"</redirme></http>";
    } else {
            *o<<"<wwwads><ads><rm>" << val.v << "<grp>" << grp.v << "</grp>"
             <<"</rm></ads></wwwads>";
    }
}

/**
*/
void ads_blk()
{
    cgi_var val, grp, blk;
    cgi_get_var("val", val);
    cgi_get_var("grp", grp);
    cgi_get_var("blk", blk);
    
    if( blk.v == "1" || blk.v == "0" ) {
            char resp = blk.v == "1" ? 1 : 0;
            if( fdwrite(so, &ADSD_ABLK1, sizeof(ADSD_ABLK1)) != sizeof(ADSD_ABLK1)
                || fdwrstr(so, grp.v) == -1
                || fdwrstr(so, val.v) == -1
                || fdwrite(so, &resp,1) == -1
                || fdread(so, &resp, sizeof(resp)) != sizeof(resp)
                || resp != 'K' ) {
                    *e << "can't " << (blk.v == "1" ? "" : "un") << "block ad.:"
                        << strerror(errno) << endl;
            }
    }
    *o <<"<http><redirme>what=2&amp;act=1&amp;grp="<<grp.v<<"</redirme></http>";
}

/**
*/
void ads_add()
{
    *o << "<wwwads><ads><add>";
    cgi_var mc, ms, ttl, misc, grp, url;
    cads::size_type mcv, msv, ttlv;

    cgi_get_var( "mc", mc );
    cgi_get_var( "ms", ms );
    cgi_get_var( "ttl", ttl );
    cgi_get_var( "misc", misc );
    cgi_get_var( "grp", grp );
    cgi_get_var( "url", url );

    if( stringsAreEqual(cgie->getRequestMethod(),"POST") ) {
            bool ok = true;
#define dirty(d,t) if(d) { *o << t; ok = false; }
            dirty( url.d, "<dirtyurl/>");
            if( url.v.empty() ) {
                    ok = false;
                    *o << "<nourl/>";
            }
            dirty( mc.d, "<dirtymc/>" );
            if( ! to_uint32_t( mc.v, mcv ) ) {
                    *o << "<nanmc/>";
                    ok = false;
            }
            dirty( ms.d, "<dirtyms/>" );
            if( ! to_uint32_t( ms.v, msv ) ) {
                    *o << "<nanms/>";
                    ok = false;
            }
            dirty( ttl.d, "<dirtyttl/>" );
            if( ! to_uint32_t( ttl.v, ttlv ) ) {
                    *o << "<nanttl/>";
                    ok = false;
            }
            dirty( misc.d, "<dirtymisc/>" );
            dirty( grp.d, "<dirtygrp/>" );
            if(grp.v.empty()) {
                    *o << "<nogrp/>";
                    ok = false;
            }
            const_file_iterator fi = cgi->getFile("file");
            if( fi == cgi->getFiles().end() ) {
                    *o << "<nofile/>";
                    ok = false;
            }
#undef dirty
            if( ok ) {
                    char resp;
                    if( fdwrite(so,&ADSD_AADD,sizeof(ADSD_AADD)) == sizeof(ADSD_AADD)
                        && fdwrstr(so, grp.v) != -1
                        && fdwrstr(so, fi->getDataType()) != -1
                        && fdwrstr(so, fi->getData()) != -1 
                        && fdwrite(so, &mcv,sizeof(mcv)) != -1
                        && fdwrite(so, &msv,sizeof(msv)) != -1
                        && fdwrite(so, &ttlv,sizeof(ttlv)) != -1
                        && fdwrstr(so, misc.v) != -1
                        && fdwrstr(so, url.v) != -1
                        && fdread(so,&resp,1) == 1
                        && resp == 'K' ) {
                            *o << "<ok/>";
                            grp.v = url.v = mc.v = ms.v = ttl.v = misc.v = "";
                    } else {
                            *o << "<cant/>";
                            *e << me << ": can't add ad.: grp=" 
                                << grp.v
                                << ": "
                                << strerror(errno) << '\n'; 
                    }
            }
    }
    if( grps_get() ) {
            deque<string>::size_type cnt = gnames.size();
            if(cnt--) {
                    *o << "<grps>";
                    do {
                            *o << "<grp><name>" << gnames[cnt]
                              << "</name><id>" << gids[cnt] 
                              << "</id>";
                            if( grp.v == gids[cnt] )
                                    *o << "<selected/>";
                            *o << "</grp>";
                    } while(cnt--);
                    *o << "</grps>";
            }
    } else *o << "<grps><cant/></grps>";
    *o   << "<mc>" << mc.v << "</mc><url>" << url.v
        << "</url><ms>" << ms.v << "</ms><ttl>"
        << ttl.v << "</ttl><misc>"
        << misc.v << "</misc></add></ads></wwwads>";
}

/**
*/
void ads_chg()
{
    cgi_var grp, val, mc, cc, cs, ms, ttl, misc, url;
    cads::size_type mcv, msv, ttlv, ccv, csv;
    char resp;

    cgi_get_var("grp",grp);
    cgi_get_var("val",val);
    cgi_get_var( "mc", mc );
    cgi_get_var( "cc", cc );
    cgi_get_var( "ms", ms );
    cgi_get_var( "cs", cs );
    cgi_get_var( "ttl", ttl );
    cgi_get_var( "misc", misc );
    cgi_get_var( "url", url );

    ostringstream out;
    out<<"<wwwads><ads><chg>";
    if( stringsAreEqual(cgie->getRequestMethod(),"POST") ) {
            bool ok = true;
#define dirty(d,t) if(d) { out << t; ok = false; }
            dirty( url.d, "<dirtyurl/>");
            if( url.v.empty() ) {
                    ok = false;
                    out << "<nourl/>";
            }
            dirty( mc.d, "<dirtymc/>" );
            if( ! to_uint32_t( mc.v, mcv ) ) {
                    out << "<nanmc/>";
                    ok = false;
            }
            dirty( cc.d, "<dirtycc/>" );
            if( ! to_uint32_t( cc.v, ccv ) ) {
                    out << "<nancc/>";
                    ok = false;
            }
            dirty( ms.d, "<dirtyms/>" );
            if( ! to_uint32_t( ms.v, msv ) ) {
                    out << "<nanms/>";
                    ok = false;
            }
            dirty( cs.d, "<dirtycs/>" );
            if( ! to_uint32_t( cs.v, csv ) ) {
                    out << "<nancs/>";
                    ok = false;
            }
            dirty( ttl.d, "<dirtyttl/>" );
            if( ! to_uint32_t( ttl.v, ttlv ) ) {
                    out << "<nanttl/>";
                    ok = false;
            }
            dirty( misc.d, "<dirtymisc/>" );
#undef dirty
            if( ok ) {
                    if( fdwrite(so,&ADSD_ACHG1,sizeof(ADSD_ACHG1)) == sizeof(ADSD_ACHG1)
                        && fdwrstr(so, grp.v) != -1
                        && fdwrstr(so, val.v) != -1
                        && fdwrstr(so, misc.v ) != -1
                        && fdwrstr(so, url.v ) != -1
                        && fdwrite(so, &mcv, sizeof(mcv)) != -1
                        && fdwrite(so, &ccv, sizeof(ccv)) != -1
                        && fdwrite(so, &msv, sizeof(msv)) != -1
                        && fdwrite(so, &csv, sizeof(csv)) != -1
                        && fdwrite(so, &ttlv, sizeof(ttlv)) != -1 
                        && fdread(so,&resp,1) == 1
                        && resp == 'K' ) {
                            *o << "<http><redirme>what=2&amp;act=1&amp;grp="
                                << grp.v << "</redirme></http>";
                            return;
                    } else {
                            out << "<cantchg/>";
                            *e << me << ": can't change ad.'s info: grp=" 
                                << grp.v << ";aid=" << val.v
                                << ": " << strerror(errno) << '\n'; 
                    }
            }
    } else {
            if( fdwrite(so, &ADSD_AINF1, sizeof(ADSD_AINF1)) == -1
                || fdwrstr(so, grp.v) == -1
                || fdwrstr(so, val.v) == -1
                || fdread(so,&resp,1) == -1
                || resp != 'F'
                || fdrdstr(so, misc.v ) == -1
                || fdrdstr(so, url.v ) == -1
                || fdread(so, &mcv, sizeof(mcv)) == -1
                || fdread(so, &ccv, sizeof(ccv)) == -1
                || fdread(so, &msv, sizeof(msv)) == -1
                || fdread(so, &csv, sizeof(csv)) == -1
                || fdread(so, &ttlv, sizeof(ttlv)) == -1 ) {
                    *e << "can't get ad.'s info: grp=" << grp.v
                        << ";aid=" << val.v << ": " << strerror(errno) << endl;
                    out << "<cant/>";
            } else {
                ostringstream os;
                os << mcv;
                mc.v = os.str(); os.str("");
                os << ccv;
                cc.v = os.str(); os.str("");
                os << msv;
                ms.v = os.str(); os.str("");
                os << csv;
                cs.v = os.str(); os.str("");
                ttlv = (ttlv - time(NULL))/(24*3600);
                os << ttlv;
                ttl.v = os.str();
            }
    }
    *o  << out.str() << "<val>" << val.v << "</val>"
        << "<grp>" << grp.v << "</grp>"
        << "<url>" << url.v << "</url>"
        << "<mc>" << mc.v << "</mc>"
        << "<cc>" << cc.v << "</cc>"
        << "<ms>" << ms.v << "</ms>"
        << "<cs>" << cs.v << "</cs>"
        << "<ttl>" << ttl.v << "</ttl>"
        << "<misc>" << misc.v << "</misc>"
        << "</chg></ads></wwwads>";
}

/**
ads processing
*/
void ads_start()
{
    const_form_iterator fi = cgi->getElement("act");
    if( fi != cgiend ) {
            string fiv = fi->getValue();
            if( fiv == "1" ) {
                    fi = cgi->getElement("op");
                    if( fi != cgiend && cgi->getElement("val") != cgiend 
                        && cgi->getElement("grp") != cgiend ) {
                            fiv = fi->getValue();
                            if( fiv == "1" ) {
                                    ads_rm();
                                    return;
                            } else if(fiv == "2" ) {
                                    ads_chg();
                                    return;
                            } else if(fiv == "3" && cgi->getElement("blk") != cgiend ) {
                                    ads_blk();
                                    return;
                            }
                    }
                    ads_ls();
            } else {
                    ads_add();
            }
    }
}

/**
begin processing requests
*/
void start1( ostream & out, ostream & err )
{
    o = &out;
    e = &err;
    cgiend = cgi->getElements().end();
    const_form_iterator fi = cgi->getElement("what");
    if( fi != cgiend ) {
            string what = fi->getValue();
            if( what == "1" ) {
                    grps_start();
                    return;
            } else if( what == "2" ) {
                    ads_start();
                    return;
            }
    }
    out << "<wwwads><main/></wwwads>";
}

/**
*/
void start( int ac, char ** av, ostream &out, ostream &err )
{   
    cgi = new Cgicc();
    cgie = & cgi->getEnvironment();
            
    // init XSLT parameters
    xsltargs1[0*2+1] = (char*) (new string(cgie->getScriptName()))->c_str();
    int now = time(NULL);
    ostringstream nowstr;
    nowstr<<hex<<now;
    xsltargs1[1*2+1] = (char *) (new string(nowstr.str()))->c_str();

    if( ! sig_pipeign() )
            throw runtime_error((string)"can't ignore SIGPIPE: "+strerror(errno));
    if( chdir(conf_adsdir.c_str()) )
            throw runtime_error((string)"can't chdir to: "+conf_adsdir+": "+strerror(errno));
    // connect to ads daemon
    if( (so=socket(AF_UNIX,SOCK_STREAM,0)) == -1 )
            throw runtime_error((string)"can't create socket: "+strerror(errno));
    strcpy(sun.sun_path, "socket");
    sun.sun_family = AF_UNIX;
    if( connect(so, (struct sockaddr*)&sun, SUN_LEN(&sun)) )
            throw runtime_error((string)"can't connect to socket: "+strerror(errno));
    // ok, let's run it
    start1( out, err );    // yup baby, it's time to party ;-)
    if(so!=-1) close(so);
    xsltargs = xsltargs1;
}

