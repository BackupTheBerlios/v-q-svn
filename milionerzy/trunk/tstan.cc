#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <fstream>

#include "tstan.h"
#include "blad.h"

TStan::TStan()
{
    struct passwd * pwd = getpwuid( getuid() );
    if( ! pwd )
            throw new Blad( (string) "nie moge pobrac informacji o uzytkowniku : " + strerror(errno));
    
    kat = (string) pwd->pw_dir + "/.milionerzy";
    
    struct stat st;
    if( stat( kat.c_str(), & st ) && errno == ENOENT
        && mkdir( kat.c_str(), S_IRWXU ) ) 
            throw new Blad( (string) "nie moge utworzyc katalogu ~/.milionerzy : " + strerror(errno) );
}

/***************************************************************************
 *
 ***************************************************************************/
TStan::~TStan()
{
}

/****************************************************************************
 * zapisuje informacje o tym ze pytanie juz sie pojawilo
 ****************************************************************************/
void TStan::zapisz( string kat, const string & pyt )
{
    kat = this->kat + "/" + kat;
    ofstream o( kat.c_str(), ios::app );
    if( ! o )
            throw new Blad( (string)"nie moge otworzyc: " + kat + " : " + strerror(errno));
    o << pyt << '\n';
    o.close();
}

/****************************************************************************
 * sprawdza czy pytanie juz sie pojawilo, jezeli tak zwraca 1, w innym wypadku 0
 ****************************************************************************/
char TStan::bylo( string kat, const string & pyt )
{
    if( kateg != kat ) {
            stany.clear();
            kateg = kat;
            kat = this->kat + "/" + kat;
            ifstream i( kat.c_str() );
            do {
                    // ignorujemy bledy
                    getline( i, kat );
                    stany.push_back( kat );
            } while( i );
            i.close();
    }
    for( unsigned i=0; i < stany.size(); i++ ) {
            if( stany[i] == pyt )
                    return 1;
    }
    return 0;
}
