#include "html.h"

using namespace std;
/**
@return true if string was changed, false if not
pass back only some characters, replace all other with HTMLBADREP
@exception logic_error throw if HTMLOK is empty
*/
bool htmlbad( string & s, const char * HTMLOK, const char HTMLBADREP ) throw(logic_error)
{
    bool chg = false;
    static char *ok = NULL;
    static uint16_t oklen;
    if( ! ok ) {
            if( ! HTMLOK || ! (oklen=HTMLOK[0]) )
                    throw logic_error("HTMLOK is empty!!!");
            uint16_t i,s;
            for( i=0, s = strlen(HTMLOK); i < s; i++ )
                    if( oklen < HTMLOK[i] )
                            oklen = HTMLOK[i];
            ok = new char[oklen+1];
            memset(ok, 0, oklen);
            for( i=0; i <= s; i++ )
                    ok[ HTMLOK[i] ] = 1;
    }
    char * str = (char *) s.c_str();
    string::size_type strl = s.length();
    for( string::size_type i =0; i < strl; i++ ) {
            if( str[i] > oklen || ! ok[str[i]] ) {
                    chg =true;
                    str[i] = HTMLBADREP;
            } 
    }
    return chg;
}
