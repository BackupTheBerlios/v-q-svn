#include "cconfmod.h"
#include "vqwww.h"

const char cconfmod::_id[] = "conf";
const char cconfmod::_menu[] = "<conf/>";
const char cconfmod::_name[] = "";

/**************************************************************************
 *
 * ************************************************************************/
char cconfmod::run() {
    const_form_iterator id = cgi->getElement("id");
    if( id != cgie && id->getValue()==_id ) {
            deque<cmod*>::size_type mod_cur, mods_cnt=mods.size();
            if(!mods_cnt) return done;
            *o << "<mod><conf>";
            for(mod_cur=0; mod_cur<mods_cnt; ++mod_cur) {
                    if( ! (mods[mod_cur]->modinfo & cmod::mi_conf) ) continue;
                    *o <<"<mod id=\"" << mods[mod_cur]->id() << "\">";
                    *o << mods[mod_cur]->conf();
                    *o << "</mod>";
            }
            *o << "</conf></mod>";
            return done;
    }
    return notme;
}
