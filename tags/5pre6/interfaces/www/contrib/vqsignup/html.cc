#include <new>
#include <inttypes.h>
#include "html.h"
#include <iostream>

using namespace std;
/**
@return true if string was changed, false if not
pass back only some characters, replace all other with HTMLBADREP
@exception logic_error throw if HTMLOK is empty
*/
bool htmlbad( string & s, const char * HTMLOK, const char HTMLBADREP )
{
    bool chg = false;
    string::size_type oklen = strlen(HTMLOK), i;
    string::iterator ptr = s.begin(), end = s.end();
    for(; ptr != end; ++ptr ) {
      for( i =0; i < oklen; i++ )
        if( *ptr == HTMLOK[i] ) break;
      
      if( i == oklen ) {
          chg = true;
          *ptr = HTMLBADREP;
      }
    }
    return chg;
}

/**
 * replaces < to &lt;, > to &gt;, " to &quot;
 * \return string with replaced chars
 */
string html_spec_enc(const string & s) {
    string ret;
    string::const_iterator cur = s.begin(), end = s.end();
    ret.reserve(s.length()*2);
    for( ; cur != end; cur++ ) {
      switch(*cur) {
      case '<':
        ret += "&lt;"; break;
      case '>':
        ret += "&gt;"; break;
      case '"':
        ret += "&quot;"; break;
      case '&':
        ret += "&amp;"; break;
      default:
        ret += *cur;
      }
    }
    return ret;
}

/**
 * does reverse action as html_spec_enc
 * \return changed string
 */
string html_spec_dec(const string & s ) {
    string ret, tmp;
    string::const_iterator cur = s.begin(), end = s.end();
    char state = 0;
    ret.reserve(s.length());
    tmp.reserve(5);

    for( ; cur!=end; cur++ ) {
      switch(state) {
      case 0:
        if( *cur == '&' ) {
          tmp = '&';
          state = 1;
        } else ret += *cur;
        break;
      case 1: // &
        tmp += *cur;
        switch(*cur) {
        case 'l':
          state = 2; break;
        case 'g':
          state = 3; break;
        case 'q':
          state = 4; break;
        case 'a':
          state = 10; break;
        default:
          ret += tmp;
          state=0;
        }
        break;
      case 2: // &l
        tmp += *cur;
        switch(*cur) {
        case 't': state = 5; break;
        default:
          ret += tmp; state = 0;
        } 
        break;
      case 3: // &g
        tmp += *cur;
        switch(*cur) {
        case 't': state = 6; break;
        default: ret+= tmp; state = 0;
        }
        break;
      case 4: // &q
        tmp += *cur;
        switch(*cur) {
        case 'u': state = 7; break;
        default: ret += tmp; state = 0;
        } 
        break;
      case 5: // &lt
        tmp += *cur;
        state = 0;
        switch(*cur) {
        case ';': tmp=""; ret += '<'; break;
        default: ret += tmp; tmp = "";
        }
        break;
      case 6: // &gt
        tmp += *cur;
        state = 0;
        switch(*cur) {
        case ';': tmp = ""; ret += '>'; break;
        default: ret += tmp; tmp = "";
        }
        break;
      case 7: // &qu
        tmp += *cur;
        switch(*cur) {
        case 'o': state = 8; break;
        default: ret += tmp; state = 0;
        }
        break;
      case 8: // &quo
        tmp += *cur;
        switch(*cur) {
        case 't': state = 9; break;
        default: ret += tmp; state = 0;
        }
        break;
      case 9: // &quot
        tmp += *cur;
        state = 0;
        switch(*cur) {
        case ';': tmp = ""; ret += '"'; break;
        default: ret += tmp; tmp = ""; 
        }
        break;
      case 10: // &a
        tmp += *cur;
        switch(*cur) {
        case 'm': state = 11; break;
        default: ret += tmp; state = 0;
        }
        break;
      case 11: // &am
        tmp += *cur;
        switch(*cur) {
        case 'p': state = 12; break;
        default: ret += tmp; state = 0;
        }
        break;
      case 12: // &amp
        tmp += *cur;
        state = 0;
        switch(*cur) {
        case ';': tmp = ""; ret += '&'; break;
        default: ret += tmp; tmp = "";
        }
        break;
      }
    }
    if( state && ! tmp.empty() ) ret += tmp;
    return ret;
}
