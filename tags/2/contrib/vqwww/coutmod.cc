#include "coutmod.h"
#include "vqwww.h"

const char coutmod::_id[] = "out";

/**************************************************************************
 *  
 *  ***********************************************************************/
char coutmod::run() {
    const_form_iterator fi = cgi->getElement("id");
    if( fi != cgie && fi->getValue() == _id ) {
            return sessclose;
    }
    return notme;
}
