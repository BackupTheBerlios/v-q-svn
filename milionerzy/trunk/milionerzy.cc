#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

#include "blad.h"
#include "tpytania.h"
#include "tstan.h"
#include "twyniki.h"

vector <TPytania> pytania;
TPytania * a_pytania = NULL;      // aktualna kategoria
TPytanie a_pytanie;               // aktualne pytanie

string wynik;
unsigned gwarant, wygrana;      // gwarantowana wygrana, oraz wygrana koncowa

TStan * stan = NULL;
TWyniki * wyniki = NULL;
bool przy, pub, pol;    // pomoce

/*************************************************************************
 * wczytuje pytania
 *************************************************************************/
void wczytaj_pyt()
{
    ifstream pyt( "pytania" );
    if( ! pyt ) 
            throw new Blad( (string) "nie moge otworzyc: pytania : " + strerror(errno) );
    string ln;
    TPytania p( stan );

    do {        
            getline( pyt, ln );
            if( ln.length() ) {
                    try { 
                            p.wczytaj( ln.c_str() );
                            if( p.ile() )
                                    pytania.push_back( p );
                            else throw new Blad( ln + " : brak pytan" );
                    } catch ( Blad * b ) {
                            cerr << "blad podczas przetwarzania pytan: " << ln << '\n';
                            throw b;
                    }
            }
    } while( pyt );
}

/************************************************************************
 * konczy gre, sprawdza czy otrzymany wynik mozna umiescic na liscie,
 * jezeli tak robi to
 ************************************************************************/
void gramy_koniec()
{
    cout << "Wygrales " << wygrana << '\n';
    if( wyniki->sprawdz( wygrana ) ) {
            string imie;
            cout << "Rekordowy wynik, podaj imie: ";
            getline( cin, imie );
            wyniki->dodaj( imie, wygrana );
    }
}

/*************************************************************************
 * pokazuje pytanie
 ************************************************************************/
void pok_pyt()
{
    cout << "Pytanie za " << a_pytania->wygrana;
    if( a_pytania->gwarant )
            cout << " (gwarantowana)";
    
    cout << '\n';
    cout << a_pytanie.pyt[0] << '\n'
         << "1) " << a_pytanie.pyt[1] << '\n'
         << "2) " << a_pytanie.pyt[2] << '\n'
         << "3) " << a_pytanie.pyt[3] << '\n'
         << "4) " << a_pytanie.pyt[4] << '\n';
}
/*************************************************************************
 * pokazuje podpowiedz pol/pol
 *************************************************************************/
void podp_pol()
{
    if( pol ) return;
    pol = true;
    if( a_pytanie.odp == "1" ) {
            a_pytanie.pyt[3] = "";
            a_pytanie.pyt[4] = "";
    } else {
            a_pytanie.pyt[1] = "";
            if( a_pytanie.odp == "2" )
                a_pytanie.pyt[3] = "";
            else a_pytanie.pyt[2] = "";
    }
    pok_pyt();
}
/*************************************************************************
 * publicznosc
 *************************************************************************/
void podp_pub()
{
    if( pub ) return;
    pub = true;
    if( (rand() & 1) )
        cout << "Publicznosc mowi: " << a_pytanie.odp << '\n';
    else cout << "Publicznosc mowi: 1\n";
}

/*************************************************************************
 *
 *************************************************************************/
void podp_przy()
{
    if( przy ) return;
    przy = true;
    cout << "Przyjaciel mowi: " << a_pytanie.odp << '\n';
}

/*************************************************************************
 * przechodzenie pytan
 *************************************************************************/
char gramy_pyt()
{
        string odp;
        unsigned i;
        
        pub = pol = przy = false;

        gwarant = wygrana = 0;
        wynik = "0";
        for( i=0; i < pytania.size(); i++ ) {
               a_pytania = & pytania[i];
               a_pytania->pytanie( a_pytanie );
               
               pok_pyt();
               do {
                    cout << "(r)ezygnuje, (p)ublicznosc, pr(z)yjaciel, p(o)l na pol, (k)oniec: ";
                    getline( cin, odp );
                    if( odp == "p" ) podp_pub();
                    else if( odp == "z" ) podp_przy();
                    else if( odp == "o" ) podp_pol();
               } while( ! cin.eof() && odp != "r" && odp != "k"
                        && odp != "1" && odp != "2" && odp != "3" && odp != "4" );
       
               if( odp == "k" ) return 1;
               if( odp == "r" ) {
                    gramy_koniec();
                    return 0;
               }
               if( odp == a_pytanie.odp ) {
                    wynik = a_pytania->kateg;
                    wygrana = a_pytania->wygrana;
                    if( a_pytania->gwarant ) gwarant = wygrana;
               } else {
                    cout << "Prawidlowa odpowiedzia jest " << a_pytanie.odp << '\n';
                    break;
               }
        }
        
        if( i != pytania.size() ) wygrana = gwarant;
        gramy_koniec();
        return 0;
}
/**************************************************************************
 * petla grania
 *************************************************************************/
void gramy ()
{
    string odp;
    do {
            cout << "nowa (g)ra, (w)yniki, (k)oniec: ";
            getline( cin, odp );
            if( odp == "g" ) {
                    if( gramy_pyt() ) return;
            } else if( odp == "w" ) {
                    vector < TWynik > w;
                    wyniki->pobierz( w );
                    for( unsigned i=0; i < w.size(); i ++ )
                            cout<< w[i].ile << '\t' << w[i].kto << '\t' <<w[i].kiedy << '\n';
            }
    } while( odp != "k" && ! cin.eof() );
}

/**************************************************************************
 *
 **************************************************************************/
int main()
{
    cout << "Milionerzy 1, Pawel Niewiadomski <new@linuxpl.org>, (C) 2002\n"
         << "Program rozprowadzany na licencji GPL, lepiej zapoznaj sie z nia.\n"
         << "Zadnych gwarancji na cokolwiek.\n"
         << "Bledy i sugestie sa mile widziane.\n";
         
    try {
            srand( time( NULL ) );
            
            stan = new TStan();
            wyniki = new TWyniki();
            if( chdir( SHARE_DIR ) )
                    throw new Blad( (string) "nie moge przejsc do: " + SHARE_DIR + strerror(errno) );
            wczytaj_pyt();
            gramy();
            delete wyniki;
            delete stan;
    } catch( Blad * e ) {
            cerr << e->what() << '\n';
            return 1;
    }
    return 0;
}
