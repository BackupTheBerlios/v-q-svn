#include <ctime>
#include <sstream>
#include <unistd.h>

#include "valid.h"

namespace valid {
		
using namespace std;

/**
 * \return empty string if NIP is valid, or "sh=\"1\"" too short, "lo=\"1\"" too long, "dirty=\"1\"" - invalid chars, "inv=\"1\"" - invalid checksum
 */
string pl_nip_valid(const string &nip) {
    string::size_type i, nipl = nip.length();
    char digs[13], digsl;
    char weigths[10] = { 6, 5, 7, 2, 3, 4, 5, 6, 7 };
    int dig, chksum=0;

    if( nipl < 10 ) return "sh=\"1\"";
    if( nipl > 13 ) return "to=\"1\"";
    for( i=0, digsl=0; i<nipl; ++i ) {
            if(nip[i]=='-') continue;
            if((dig=nip[i]-'0') < 0 || dig > 9) return "dirty=\"1\"";
            digs[digsl++] = dig;
    }
    if(digsl!=10) return "inv=\"1\"";
    for( i=0; i < 10; ++i )
            chksum += weigths[i] * digs[i];    
    if( chksum % 11 != digs[9] ) return "inv=\"1\"";
    return "";
}

/**
 * \return empty string if Post Code is valid, or string including "inv=\"1\""
 */
string pl_pcode_valid(const string &code) {
    if(code.length()!=6 || code[2]!='-') return "inv=\"1\"";
    int dig;
    if( (dig = code[0]-'0') < 0 || dig > 9
       || (dig = code[1]-'0') < 0 || dig > 9
       || (dig = code[3]-'0') < 0 || dig > 9
       || (dig = code[4]-'0') < 0 || dig > 9
       || (dig = code[5]-'0') < 0 || dig > 9 ) return "inv=\"1\"";
    return "";
}

/**
 *
 */
string email_valid(const string &) {
    string ret;
    return ret;
}

/**
 *
 */
string date_valid( string date ) {
 const char *inv = "inv=\"1\"";
 string ret;
 string::size_type hyp, hyp1;
 istringstream is;
 int tmp;
 time_t tt = time(NULL);
 struct tm *t = gmtime(&tt);
         
 hyp = date.find('-');
 hyp1 = date.rfind('-');
 if( hyp == hyp1 || hyp == string::npos || hyp+1 == hyp1 ) return inv;
 
 is.str( date.substr(0, hyp) );
 is>>tmp;
 if( ! is ) return inv;
 tmp = t->tm_year - (tmp - 1900);
 if( tmp < 5 || tmp > 100 ) {
   ostringstream os;
   os<<t->tm_year;
   return os.str();
 }
 
 is.str( date.substr(hyp+1, hyp1-hyp) );
 is>>tmp;
 if( ! is ) return inv;
 if( tmp < 1 || tmp > 12 ) return inv;

 is.str( date.substr(hyp1+1) );
 is>>tmp;
 if( ! is ) return inv;
 if( tmp < 1 || tmp > 31 ) return inv;
 return ret;
}

} // namespace valid
