#include <fstream>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include "twyniki.h"
#include "blad.h"

/**************************************************************************
 *
 *************************************************************************/
void TWynik::ustaw_kiedy()
{
    char b[11];
    time_t t = time(NULL);
    strftime( b, sizeof(b), "%Y-%m-%d", gmtime( & t ));
    kiedy = b;
}

/*************************************************************************
 *
 *************************************************************************/
char TWyniki::sprawdz( unsigned ile )
{
    if( ! ile ) return 0;
    vector < TWynik > stare;
    pobierz( stare );

    for( unsigned i=0; i < stare.size(); i++ ) {
            if( stare[i].ile < ile ) {
                    return 1;
            }
    }
    if( stare.size() < this->ile )
            return 1;

    return 0;
}
/************************************************************************
 *
 ************************************************************************/
void TWyniki::dodaj( const string & kto, unsigned ile )
{
    vector < TWynik > stare;
    pobierz( stare );
    
    fstream p( "wyniki", ios::out | ios::trunc, 0666 );
    if( ! p )
            throw new Blad( (string) "nie moge otworzyc pliku wyniki : " + strerror(errno) );

    unsigned i;
    for( i=0; i < stare.size(); i++ ) {
            if( stare[i].ile < ile ) {
                    wyn.ile = ile;
                    wyn.kto = kto;
                    wyn.ustaw_kiedy();
                    stare.insert( stare.begin()+i, wyn );
                    break;
            }
    }
    if( i == stare.size() && i < this->ile ) {
            wyn.ile = ile;
            wyn.kto = kto;
            wyn.ustaw_kiedy();
            stare.push_back( wyn );
    }
    if( stare.size() > this->ile )
            stare.erase( stare.begin()+this->ile, stare.end() );
    
    for( i=0; i < stare.size(); i++ ) {
            p.write( & stare[i].ile, sizeof(stare[i].ile) );
            p << stare[i].kto << '\0'
              << stare[i].kiedy << '\0';
    }
    p.close();
}

/**************************************************************************
 *
 **************************************************************************/
void TWyniki::pobierz( fstream & p, vector< TWynik > & w )
{
    w.erase( w.begin(), w.end() );
    do {
        p.read( & wyn.ile, sizeof( wyn.ile ) );
        getline( p, wyn.kto, '\0' );
        getline( p, wyn.kiedy, '\0' );
        if( p.eof() ) break;
        w.push_back( wyn );
    } while( ! p.eof() && p );
}

/*************************************************************************
 *
 *************************************************************************/
void TWyniki::pobierz( vector < TWynik > & w )
{
    fstream p( "wyniki", ios::nocreate | ios::in );
    if( p ) pobierz( p, w );
    p.close();
}
