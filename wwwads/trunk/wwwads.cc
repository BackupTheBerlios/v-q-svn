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

#include <iostream>
#include <string>
#include <stdexcept>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <cgicc/Cgicc.h>

#include "adsd.h"
#include "sig.h"
#include "conf_adsdir.h"
#include "conf_wwwdir.h"
#include "conf_adsalt.h"
#include "getline.h"
#include "fd.h"
#include "fdstr.h"

using namespace cgicc;
using namespace std;

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

/**
*/
void show(const string &gid, const string &aid)
{
    if( fdwrite(so, &ADSD_AGETURL, sizeof(ADSD_AGETURL)) != sizeof(ADSD_AGETURL)
        || fdwrstr(so, gid) == -1
        || fdwrstr(so, aid) == -1 )
            throw runtime_error((string)"can't write to daemon: "+strerror(errno));
    char resp;
    switch( fdread(so,&resp,1) ) {
    case 0: throw runtime_error((string)"eof while reading from daemon");
    case -1: throw runtime_error((string)"error while reading from daemon: "+strerror(errno));
    }
    if(resp=='F') {
            string url;
            if( fdrdstr(so,url) != -1 ) {
                    cout << "Location: " << url << "\n\n";
            } else {
                    throw runtime_error((string) "error while reading from daemon: "+strerror(errno));
            }
    } else if( resp == 'E' ) {
            throw runtime_error("error in daemon mode");
    } else {
            throw runtime_error((string)"unrecognized response: "+resp);
    }
}
/**
*/
void start()
{
    const_form_iterator igid = cgi->getElement("gid");
    const_form_iterator iaid = cgi->getElement("aid");
    string gid, aid;
    if( igid != cgiend ) {
            gid = igid->getValue();
    }
    if( iaid != cgiend ) {
            aid = iaid->getValue();
    }
    if( ! gid.empty() && ! aid.empty() && cgi->getElement("show") != cgiend ) {
            show(gid,aid);
            return;
    }
    if( fdwrite(so, &ADSD_AGET, sizeof(ADSD_AGET)) != sizeof(ADSD_AGET)
        || fdwrstr(so, gid) == -1
        || fdwrstr(so, aid) == -1 )
            throw runtime_error((string)"can't write to daemon: "+strerror(errno));

    char resp;
    switch( fdread(so,&resp,1) ) {
    case 0: throw runtime_error((string)"eof while reading from daemon");
    case -1: throw runtime_error((string)"error while reading from daemon: "+strerror(errno));
    }
    if(resp=='F') {
            string url;
            if( fdrdstr(so,url) != -1
                && fdrdstr(so,gid) != -1
                && fdrdstr(so,aid) != -1 ) {
                    cout<< "\n<A HREF=\"" << cgie->getScriptName() 
                        << "?gid=" << gid
                        << "&aid=" << aid
                        << "&show="
                        << "\"><IMG SRC=\"" << conf_wwwdir << url << "\" ALT=\"" 
                        << conf_adsalt << "\" BORDER=0>"
                        << "</A>";
            } else {
                    throw runtime_error((string) "error while reading from daemon: "+strerror(errno));
            }
    } else if( resp == 'E' ) {
            throw runtime_error("error in daemon mode");
    } else {
            throw runtime_error((string)"unrecognized response: "+resp);
    }
}

/**
*/
int main( int ac, char ** av )
{
    me = *av;
    cout<< "Content-Type: text/html\n";
    try {
            cgi = new Cgicc();
            cgie = & cgi->getEnvironment();
            cgiend = cgi->getElements().end();
            
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
            start();
    } catch( exception & e ) {
            cerr << '\n' << *av << ": exception: " << e.what() << '\n';
            return 1;
    }
    return 0;
}
