/*
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
