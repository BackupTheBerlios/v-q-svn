/**
Copyright (c) 2002,2003 Pawel Niewiadomski
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

#include <new>
#include <unistd.h>
#include <libgen.h>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <cerrno>
#include <map>

#include <sablot.h>

#include "vquserrm_conf.h"
#include "sabmain.h"
#include "conf_home.h"

using namespace std;
using namespace vquserrm;

static const char *me = NULL;
const char** xsltargs=NULL;

/**************************************************************************
 * Sablotron message handlers
 **************************************************************************/

/// don't know what it does
MH_ERROR sabmsg_makecode( void * userdata, SablotHandle sh, int severity,
        unsigned short f, unsigned short code )
{
        return 100;
}

/// log message
MH_ERROR sabmsg_log( void * userdata, SablotHandle sh, MH_ERROR code, 
        MH_LEVEL lev, char ** fields )
{
#if 0
        cerr << me << ": SLOG: ";
        while( *fields ) {
                cerr << *fields << ';';
                fields++;
        }
        cerr << '\n';
#endif
        return 0;
}

/// error message
MH_ERROR sabmsg_error( void * userdata, SablotHandle sh, MH_ERROR code,
        MH_LEVEL lev, char ** fields )
{
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
int main( int ac, char ** av )
{   
    me = *av;

	SablotSituation sit;
    SablotHandle sh;
    if( SablotCreateSituation(&sit)
		|| SablotCreateProcessorForSituation(sit, &sh)
        || SablotRegHandler(sh, HLR_MESSAGE, &sabmsghan, NULL ) ) {
            cerr << me << ": can't initialize Sablotron\n";
            return 111;
    }
   
	string xslt_dir(conf_home+"/share/vquserrm");
    string xslt_fn(xslt_dir+"/vquserrm.xslt");
    
	ostringstream out;
	xslt_args_type xslt_args;
    try {
            out << ac_xmlstart.val_str() << endl;
            start( ac, av, out, cerr, xslt_args );
    } catch( const exception & e ) {
            cerr << *av << ": exception cougth: " << e.what() << endl;
            out.clear();
            out.str("");
            out << ac_xmlstart.val_str() << endl
                << "<exception>" << e.what() << "</exception>" << endl;
    } catch( ... ) {
            cerr << *av << ": unknown exception\n";
            out.clear();
            out.str("");
            out << ac_xmlstart.val_str() << endl
                << "<exception unexpected=\"1\"/>" << endl;
    }

	if( chdir(xslt_dir.c_str()) ) {
			cerr<<me<<": chdir to: "<<xslt_dir<<": "<<strerror(errno)<<endl;
			return 111;
	}

	if( ! xslt_args.empty() ) {
			xslt_args_type::const_iterator cur, end;
			for( cur=xslt_args.begin(), end=xslt_args.end(); cur!=end; ++cur ) {
					if( SablotAddParam(sit, sh, cur->first.c_str(), 
								cur->second.c_str()) ) {
            			cerr<<me << ": can't set XSLT param: "<<cur->first
							<<": "<<cur->second<<endl;
					}
			}
					
	}
    if( SablotAddArgBuffer(sit, sh, "xml", out.str().c_str())
		|| SablotRunProcessorGen( sit, sh, xslt_fn.c_str(), "arg:/xml",
                "file:///__stdout" ) ) {
            cerr << me << ": error while converting output\n";
            cerr << '"' << out.str().c_str() << '"' << endl;
            return 111;
    }
	return 0;
}

