#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <stdlib.h>
#include <dirent.h>

#include "tpytania.h"
#include "blad.h"

/*****************************************************************************
 *
 *****************************************************************************/
TPytania::TPytania( TStan * s )
{
    stan = s;
    wygrana = 0;
}

/*****************************************************************************
 *
 *****************************************************************************/
TPytania::TPytania( const char * dir, TStan * s )
{
    stan = s;
    wygrana = 0;
    wczytaj( dir );
}

/****************************************************************************
 *
 ****************************************************************************/
void TPytania::ustaw_gwarant( const char * dir, int fd )
{
    struct stat st;
    if( fstat( fd, & st ) ) 
            throw new Blad( (string) "nie moge odczytac informacji o :" + dir );
    if( ( st.st_mode & S_ISVTX ) )
            gwarant = true;
    else gwarant = false;
}

/****************************************************************************
 *
 ****************************************************************************/
void TPytania::wczytaj_pyt( DIR * kat )
{
    struct dirent * ent;
    struct stat st;

    pytania.clear();
    while( (ent=readdir( kat )) ) {
            if( stat( ent->d_name, & st ) )
                    throw new Blad( (string) "stat: " + ent->d_name + " : " + strerror(errno) );
            if( ! ( st.st_mode & S_IFDIR ) ) 
                    dodaj_pyt( ent->d_name );
    }
}   

/****************************************************************************
 *
 ****************************************************************************/
void TPytania::ustaw_wygrana( const char * d )
{
    char * ep;
    long t = strtol( d, & ep, 10 );
    if( ! *d || *ep != '\0' || t < 0 )
            throw new Blad( (string) "nieprawidlowa nazwa kategorii: " + d );
    wygrana = t;
}

/****************************************************************************
 *
 *****************************************************************************/
void TPytania::wczytaj( const char * dir )
{
    ustaw_wygrana( dir );
    kateg = dir;
    DIR * akt = opendir( "." ), * kat;
    if( ! akt ) throw new Blad( "nie moge otworzyc aktualnego katalogu" );
    if( chdir( dir ) ) throw new Blad( (string) "nie moge przejsc do : " + dir );
    
    kat = opendir( "." );
    if( ! kat )
            throw new Blad( (string) "nie moge otworzyc: " + dir + (string) " : " + strerror(errno) );
    ustaw_gwarant( dir, dirfd(kat) );
    
    wczytaj_pyt( kat );
    closedir( kat );
    
    if( fchdir( dirfd(akt) ) ) {
            closedir( akt );
            throw new Blad( "nie moge wrocic do katalogu" );
    }
    closedir( akt );
}

/*****************************************************************************
 *
 *****************************************************************************/
TPytania::~TPytania()
{
}

/***************************************************************************
 * dodaje pytanie do listy
 ***************************************************************************/
void TPytania::dodaj_pyt( const string & fn )
{
    if( stan && stan->bylo( kateg, (string) fn ) ) return;

    ifstream in( fn.c_str(), ios::in | ios::nocreate );
    if( ! in )
            throw new Blad( (string) "nie moge otworzyc : " + fn + " : " + strerror(errno) );
    
    getline( in, pyt.odp );
    if( pyt.odp != "1" && pyt.odp != "2" && pyt.odp != "3" && pyt.odp != "4" )
            throw new Blad( (string) "plik " + fn + " ma nieprawidlowy format" );
    
    getline( in, pyt.pyt[0] );
    getline( in, pyt.pyt[1] );
    getline( in, pyt.pyt[2] );
    getline( in, pyt.pyt[3] );
    getline( in, pyt.pyt[4] );
    if( ! in ) throw new Blad( (string) "plik: " + fn + " : " + strerror(errno) ); 
    
    if( pyt.pyt[0] == "" || pyt.pyt[1] == "" 
        || pyt.pyt[2] == "" || pyt.pyt[3] == "" || pyt.pyt[4] == "" )
            throw new Blad( (string) "plik " + fn + " ma nieprawidlowy format" );
  
    in.close(); 
    pyt.plik = fn;
    pytania.push_back( pyt );
}

/**************************************************************************
 * pobiera jakies pytanie, zaznacza je jako pokazane
 **************************************************************************/
void TPytania::pytanie( TPytanie & p )
{
    if( ! pytania.size() )
            throw new Blad( (string) "brak pytan z kategorii " + kateg );
    
    unsigned nr = (unsigned)((double) rand()/((double)RAND_MAX+1) * (pytania.size()-1));
    p = pytania[nr];
    if( stan ) 
            stan->zapisz( kateg, p.plik );
    pytania.erase( pytania.begin() + nr );
}
