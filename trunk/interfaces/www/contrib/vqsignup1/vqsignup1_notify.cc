/**
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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
#include "conf_home.h"
#include "vqsignup1_conf.h"
#include "html.h"

#include <util.h>
#include <lower.h>
#include <fd.h>
#include <sig.h>

#define HAVE_NAMESPACE_STD 1
#include <pqxx/all.h>

#include <sablot.h>

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

using namespace std;
using namespace vqsignup1;

static char *me = NULL;
const char** xsltargs=NULL;
pqxx::Connection *pg = NULL;
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
	string xslt_fn(conf_home+"/share/vqsignup1/vqsignup1_notify.xslt");
    ifstream xslt( xslt_fn.c_str() );
    if( ! xslt )
            throw runtime_error((string)"can't open XSLT file: " 
          		+ xslt_fn + " : " + strerror(errno));
    string tmp;
    while( getline( xslt, tmp ) ) {
            xsltsheet += tmp + "\n";
    }
    if( xslt.bad() )
            throw runtime_error((string)"error while reading XSLT file: " 
            	+ xslt_fn + " : "+strerror(errno));
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
    tb = str2tb(dom);
    rcpt = ac > 4 ? av[4] : (string)av[2]+'@'+av[1];

    sig_pipe_ign();
    try {
            pg = new pqxx::Connection(ac_pgsql.val_str().c_str());

			pqxx::Result res(pqxx::NonTransaction(*pg).Exec(
				"SELECT * FROM users_info,"+tb
				+" WHERE users_info.id="
				+pqxx::Quote((string)av[3], false)+" AND "
				+tb+".login=users_info.login LIMIT 1") );

			if( res.size() == 1 ) {
					ostringstream out;
					int field, fields = res[0].size();
					string lf;
                    
                    out<<ac_xmlstart1.val_str()<<endl;
                    out<<"<vqsignup1_notify>";
                    out<<"<new><user>" << user
                       << "</user><domain>" << dom
                       <<"</domain></new>";
					
                    for( field=0; field < fields; ++field ) {
                            lf = lower(res[0][field].c_str());
                            out<<"<"<<lf<<">";
                            out<<html_spec_enc(res[0][field].c_str());
                            out<<"</"<<lf<<">";
                    }
                    out<<"</vqsignup1_notify>";

                    delete pg;
                  	qi(xml2out(out.str()));
			}
    } catch( const exception & e ) {
            cerr<<me<<": exception: "<<e.what();
            return 1;
    }
    return 0;
}

