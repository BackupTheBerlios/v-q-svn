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

#include <sys/types.h>
#include <sys/wait.h>
#include <new>
#include <libgen.h>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <errno.h>
#include <unistd.h>

#include <libpq-fe.h>
#include <libpq++.h>

#include <sablot.h>

#include <util.h>
#include <lower.h>
#include <fd.h>
#include <sig.h>

#include "conf_xmlstart1.h"
#include "conf_xsltsheet1.h"
#include "conf_pgsql.h"
#include "html.h"

using namespace std;

static char *me = NULL;
const char** xsltargs=NULL;
PgDatabase *pg = NULL;
string rcpt;

/**************************************************************************
 * Sablotron message handlers
 **************************************************************************/

/// don't know what it does
MH_ERROR sabmsg_makecode( void * userdata, SablotHandle sh, int severity,
        unsigned short f, unsigned short code ) {
    return 100;
}

/// log message
MH_ERROR sabmsg_log( void * userdata, SablotHandle sh, MH_ERROR code, 
        MH_LEVEL lev, char ** fields ) {
    return 0;
}

/// error message
MH_ERROR sabmsg_error( void * userdata, SablotHandle sh, MH_ERROR code,
        MH_LEVEL lev, char ** fields ) {
    cerr << me << ": SERR: ";
    while( *fields ) {
            cerr << *fields << ';';
            fields++;
    }
    cerr << '\n';
    return 0;
}

/// sablotron message handlers
MessageHandler sabmsghan = {
    sabmsg_makecode,
    sabmsg_log,
    sabmsg_error
};

/**
*/
string xml2out( const string & xmlin ) {
    string ret;
    SablotHandle sh;
    
    if( SablotCreateProcessor(&sh)
        || SablotRegHandler(sh, HLR_MESSAGE, &sabmsghan, NULL ) )
            throw runtime_error("can't initialize Sablotron");
    
    string xsltsheet;
    ifstream xslt( conf_xsltsheet1.c_str() );
    if( ! xslt )
            throw runtime_error((string)"can't open XSLT file: " 
                    + conf_xsltsheet1 + " : " + strerror(errno));
    string tmp;
    while( getline( xslt, tmp ) ) {
            xsltsheet += tmp + "\n";
    }
    if( xslt.bad() )
            throw runtime_error((string)"error while reading XSLT file: " 
                    +conf_xsltsheet1 + " : "+strerror(errno));
    xslt.close();

    const char *sabargs[] = { 
            "/_xmlinput", xmlin.c_str(),
            "/_stylesheet", xsltsheet.c_str(),
            "/_output", NULL,
            NULL
    };

    char *text;
    if( SablotRunProcessor( sh, "arg:/_stylesheet", "arg:/_xmlinput",
                "arg:/_output", xsltargs, sabargs)
       || SablotGetResultArg( sh, "arg:/_output", &text )
       || (ret = text).empty()
       || SablotFreeResultArgs(sh)
       || SablotDestroyProcessor(sh) ) {
            throw runtime_error((string)"error while converting output: "
                    + xmlin );
    }
    return ret;
}

/**
 */
void qi(const string &msg) {
    int pid, pp[2];
    if(pipe(pp)==-1) throw runtime_error((string)"pipe failed: "+strerror(errno));
    
    switch((pid=vfork())) {
    case -1: throw bad_alloc();
    case 0:
      close(pp[1]);
      if( dup2(pp[0], 0) == -1 ) _exit(111);
      execlp("qmail-inject", "qmail-inject","-H",(char *) rcpt.c_str(), NULL);
      _exit(111);
    }
    close(pp[0]);
    if( fdwrite(pp[1], msg.data(), msg.length()) == -1 ) {
            cerr << me << ": write: "<< strerror(errno);
    }
    close(pp[1]);
    while(wait(&pid) == -1 && errno == EINTR );
}

/**
*/
int main( int ac, char ** av )
{   
    me = *av;
    string tb, user, dom;
    if( ac < 4 ) {
            cerr<<"usage: "<<me<<" domain user id [rcpt@domain]"<<endl;
            return 2;
    }
    dom = lower((string)av[1]);
    user = lower((string)av[2]);
    tb = dom;
    str2tb(tb);
    rcpt = ac > 4 ? av[4] : (string)av[2]+'@'+av[1];

    sig_pipeign();
    try {
            pg = new PgDatabase(conf_pgsql.c_str());
            if( pg->ConnectionBad() )
                    throw runtime_error((string)"can't connect to PostgreSQL: "+pg->ErrorMessage());

            if( pg->Exec(((string) "SELECT * FROM users_info,"+tb
                    +" WHERE users_info.id='"
                    +escape(av[3])+"' AND "
                    +tb+".login=users_info.login LIMIT 1").c_str()) == PGRES_TUPLES_OK
                && pg->Tuples() == 1 ) {
                    ostringstream out;
                    int field, fields = pg->Fields();
                    string lf;
                    
                    out<<conf_xmlstart1<<endl;
                    out<<"<vqsignup1_notify>";
                    out<<"<new><user>" << user
                       << "</user><domain>" << dom
                       <<"</domain></new>";
                    for( field=0; field < fields; ++field ) {
                            lf = lower(pg->FieldName(field));
                            out<<"<"<<lf<<">";
                            out<<html_spec_enc(pg->GetValue(0,field));
                            out<<"</"<<lf<<">";
                    }
                    out<<"</vqsignup1_notify>";

                    delete pg;
                    qi(xml2out(out.str()));
            } else { 
                    cerr<<me<<": select: "<<pg->ErrorMessage()<<endl;
                    return 1;
            }
    } catch( const exception & e ) {
            cerr<<me<<": exception: "<<e.what();
            return 1;
    }

    return 0;
}

