/*
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
*/

#include <unistd.h>
#include <stdexcept>
#include <errno.h>
#include <fcntl.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>

#include "cdiskads.h"
#include "cdiskadstest.h"
#include "conf-cdiskads"

void cdiskadstest::run()
{
    test_( init() );
    // normal operation
    test_(grps_add("test"));
    test_(grps_add("fff"));
    test_(grps_add("test"));
    deque<string> gids, gnames;
    test_(grps(gnames,gids));
    test_(gids.size()==3);
    if( gids.size() == 3 ) {
            test_(gnames[0]=="test");
            test_(gnames[1]=="fff");
            test_(gnames[2]=="test");
    }
    deque<string> deq;
    deq.push_back(gids[0]);
    test_(grps_rm(deq));
    test_(ads_add(gids[1], "Content-Type: text/plain\n\ntest\n", 1, 1, 34, "misc", "url"));
    test_(ads_add(gids[2], "Content-Type: text/plain\n\ntest\n", 1, 1, 34, "misc", "url"));
    string tmp, tmp1, tmp2;
    test_(ads_get_grp_ad(gids[1], "00000000", tmp));
    test_(tmp==((string)+"/"+gids[1]+"/00000000"+ASISEXT));
    test_(ads_get_grp(gids[1],tmp,tmp1));
    test_(tmp1==((string)+"/"+gids[1]+"/00000000"+ASISEXT));
    test_(ads_get(tmp,tmp1,tmp2));
    test_(tmp2==((string)"/"+gids[2]+"/00000000"+ASISEXT));
    test_(!ads_get_grp("234dsf",tmp,tmp1));
    test_(tmp1=="");
    cdiskads::ad_info ai;
    ai.aid = "00000000";
    test_(ads_info("01000000",ai));
    deque <cdiskads::ad_info> ais;
    test_(ads_ls("01000000",ais));
    test_(ads_ls("02000000",ais));
    test_(!ads_ls("01230000",ais));
    deq.clear();
    deq.push_back((string)"00000000");
//    test_(ads_rm("02000000",deq));
//    test_(grps_rm(gids));
    // check errors*/
}

/**
*/
void cdiskadstest::do_fail(const string& lbl, const char* fname, long lineno)
{
        Test::do_fail(lbl,fname,lineno);
        *osptr<<"err: " << errmsg << '\n';        
}

