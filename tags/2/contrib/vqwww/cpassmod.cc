#include "cpassmod.h"
#include "vqwww.h"
#include "cgi.h"
#include "conf_pass_maxl.h"
#include "conf_pass_minl.h"
#include "conf_pass_ok.h"
#include "conf_html_br.h"
#include "html.h"

const char cpassmod::_id[] = "pass";
const char cpassmod::_conf[] = "<pass/><desc><pass/></desc>";
const char cpassmod::_name[] = "";

void cpassmod::act() {
    *o << "<mod><pass>";
    if( reqmet == post ) {
            bool ok= true;
            string cpass, npass, rpass;
            string::size_type len;
            *o<<"<inv>";
            *o<<"<cpass";
            if( ! cgi_var(cgi, "cpass", cpass) || cpass.empty() ) {
                    ok = false;
                    *o<<" empty=\"1\" "; 
            }
            *o<<"/>";
            *o<<"<npass";
            if( ! cgi_var(cgi, "npass", npass) || npass.empty() ) {
                    ok = false;
                    *o<<" empty=\"1\" ";
            } else if( (len=npass.length()) < conf_pass_minl_i ) {
                    ok = false;
                    *o<<" tooshort=\"1\" ";
            } else if( len > conf_pass_maxl_i ) {
                    ok = false;
                    *o<<" toolong=\"1\" ";
            } else if( htmlbad(npass, conf_pass_ok.c_str(), conf_html_br[0] ) ) {
                    ok = false;
                    *o << " dirty=\"1\" ";
            }
            *o<<"/>";
            cgi_var(cgi, "rpass", rpass);
            if( rpass != npass ) {
                    ok = false;
                    *o<<"<dontmatch/>";
            }
            if( ok ) {
                    if( ! vq->user_pass(sessv.ai.user, sessv.ai.dom, npass ) ) {
                            *o<<"<done/>";
                    } else {
                            *o<<"<cant/>";
                            *e<<"cpassmod: can't change password: "<<vq->err()<<endl;
                    }
            }
            *o<<"</inv>";
    }
    *o << "</pass></mod>";
}
/**************************************************************************
 *  
 *  ***********************************************************************/
char cpassmod::run() {
    const_form_iterator fi = cgi->getElement("conf");
    if( fi != cgie ) {
            fi = cgi->getElement("id");
            if( fi != cgie && fi->getValue() == _id ) {
                    act();
                    return done;
            }
    }
    return notme;
}
