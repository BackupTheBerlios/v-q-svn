/*
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

#include "cconfmod.h"
#include "vqwww.h"

using namespace std;
using namespace cgicc;

const char cconfmod::_id[] = "conf";
const char cconfmod::_menu[] = "<conf/>";

/**************************************************************************
 *
 * ************************************************************************/
char cconfmod::run() {
    const_form_iterator id = env.cgi_get().getElement("id");
    if( id != env.cgi_end_get() && id->getValue()==_id ) {
            deque<cmod*>::size_type mod_cur, mods_cnt=env.mods.size();
            if(!mods_cnt) return done;
            env.out_get()<< "<mod><conf>";
            for(mod_cur=0; mod_cur<mods_cnt; ++mod_cur) {
                    if( ! (env.mods[mod_cur]->modinfo & cmod::mi_conf) ) continue;
                    env.out_get()<<"<mod id=\"" << env.mods[mod_cur]->id() << "\">";
                    env.out_get()<< env.mods[mod_cur]->conf();
                    env.out_get()<< "</mod>";
            }
            env.out_get()<< "</conf></mod>";
            return done;
    }
    return notme;
}
