/**
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

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

#include "vqsignup1_conf.h"
#include "sabmain.h"
#include "conf_home.h"

using namespace std;
using namespace vqsignup1;

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
   
	string xslt_dir(conf_home+"/share/vqsignup1");
    string xslt_fn(xslt_dir+"/vqsignup1.xslt");
    
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

