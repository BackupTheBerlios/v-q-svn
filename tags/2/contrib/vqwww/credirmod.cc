#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>

#include "credirmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "conf_redir_max.h"

using namespace std;

const char credirmod::_id[] = "redir";
const char credirmod::_conf[] = "<redir/><desc><redir/></desc>";
const char credirmod::_name[] = "";

void credirmod::rmspace(string &s) {
 string::iterator ptr=s.begin(), end=s.end();
 while( ptr != end ) {
   if( isspace(*ptr) ) {
     s.erase(ptr);
     end = s.end();
   } else ptr ++;
 }
}
/***************************************************************************
 * 
 * *************************************************************************/
void credirmod::act() {
 vector<cvq::udot_info> uis;
 *o << "<mod><redir>";
 if(! vq->udot_ls(sessv.ai.dom, sessv.ai.user, "", cvq::redir, uis ) ) {
   vector<cvq::udot_info>::size_type cnt = uis.size(), i;
   if( reqmet == post ) {
     bool ok= true;
     string val, id;
     vector<cvq::udot_info> torep, toadd, torm(uis);
     vector<cvq::udot_info>::iterator ptr;
     cvq::udot_info uitmp;
     ostringstream os;
     
     *o<<"<inv>";
     // we get list of items
     if( conf_redir_max_i > cnt ) cnt = conf_redir_max_i;
     for( i=1; i <= cnt; i++ ) {
       os.str("");
       os<<i;
       if( cgi_var(cgi, "item"+os.str(), val) ) {
         cgi_var(cgi, "itemid"+os.str(), id);
         
         rmspace(val);
         if( val.empty() ) continue;
         rmspace(id);

         cmp.id = id;
         if( ! cmp.id.empty()
            && (ptr=find_if(torm.begin(), torm.end(), cmp)) != torm.end() ) {
           if ( ptr->val != val ) {
             uitmp.type = cvq::redir;
             uitmp.val = val;
             uitmp.id = id;
             torep.push_back(uitmp);
           }
           torm.erase(ptr);
         } else {
           uitmp.type = cvq::redir;
           uitmp.val = val;
           toadd.push_back(uitmp);
         }
       }
     }
     if( ok ) {
       for( i=0, cnt=toadd.size(); i<cnt; ++i ) {
         if(vq->udot_add(sessv.ai.dom, sessv.ai.user, "", toadd[i])) {
                 *e<<"credirmod::udot_add: "<<vq->err()<<endl;
                 *o<<"<add>"<< toadd[i].val <<"</add>";
         }
         if(vq->udot_rm(sessv.ai.dom, sessv.ai.user, "", cvq::maildir) ) {
                *e<<"credirmod::udot_rm: "<<vq->err()<<endl;
         }
         uis.push_back(toadd[i]);
         
       }
       for( i=0, cnt=torep.size(); i<cnt; ++i ) {
         if(vq->udot_rep(sessv.ai.dom, sessv.ai.user, "", torep[i])) {
                 *o << "<rep>"<< torep[i].val <<"</rep>";
                 *e<<"credirmod::udot_rep: "<<vq->err()<<endl;
         }
         cmp.id = torep[i].id;
         ptr = find_if(uis.begin(), uis.end(), cmp);
         if( ptr != uis.end() ) *ptr = torep[i];
       }
       for( i=0, cnt=torm.size(); i<cnt; ++i ) {
         cvq::size_type udot_cnt;
         if( vq->udot_type_cnt(sessv.ai.dom, sessv.ai.user, "", cvq::redir, udot_cnt ) ) {
                 *e<<"credirmod::udot_type_cnt: "<<vq->err()<<endl;
                 continue;
         }
         if( udot_cnt <= 1
             && !vq->udot_has(sessv.ai.dom, sessv.ai.user, "", cvq::maildir ) ) {
                switch( vq->udot_add_md_def(sessv.ai.dom, sessv.ai.user, "") ) {
                case cvq::err_udot_ex:
                case cvq::err_no:
                        break;
                default: 
                        *e<<"credirmod::udot_add_md_def: "<<vq->err()<<endl;
                        continue;
                }
         }
         if( vq->udot_rm(sessv.ai.dom, sessv.ai.user, "", torm[i].id)) {
                 *o << "<rm>"<<torm[i].val<<"</rm>";
                 *e<<"credirmod::udot_rm: "<<vq->err()<<endl;
                 continue;
         }
         cmp.id = torm[i].id;
         ptr = find_if(uis.begin(), uis.end(), cmp);
         if( ptr != uis.end() ) uis.erase(ptr);
       }
     }
     *o<<"</inv>";
   }
   for( i=0, cnt = uis.size(); i<cnt; ++i )
     *o <<"<item val=\"" << uis[i].val << "\" id=\""
        << uis[i].id << "\"/>";
   if( cnt < conf_redir_max_i ) {
     cnt = conf_redir_max_i - cnt;
     for( i=0; i < cnt; ++i )
       *o <<"<item/>";
   }
 } else *o << "<getcant/>";
 *o << "</redir></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char credirmod::run() {
 const_form_iterator fi = cgi->getElement("conf");
 if( fi != cgie ) {
   fi = cgi->getElement("id");
   if( fi != cgie && fi->getValue() == _id ) {
     if( vq->udot_sup_is(cvq::redir) && vq->udot_sup_is(cvq::maildir) ) {
       act();
       return done;
     }
   }
 }
 return notme;
}
