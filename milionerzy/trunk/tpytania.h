#ifndef _TPYTANIA_H
#define _TPYTANIA_H

#include <sys/types.h>
#include <dirent.h>

#include <vector>
#include <string>

#include "tstan.h"

class TPytanie {
        public:
                string pyt[5], plik, odp;
};

class TPytania {
        protected:
                vector <TPytanie> pytania;
                TStan * stan;
                TPytanie pyt;
                        
                void dodaj_pyt( const string & );
                void ustaw_gwarant( const char *, int );
                void wczytaj_pyt( DIR * );
                void ustaw_wygrana( const char * );
        public:
                unsigned wygrana;
                string kateg;
                bool gwarant;
                
                TPytania( TStan * );
                TPytania( const char *, TStan * );
                ~TPytania();    
                unsigned ile() { return pytania.size(); }
                void wczytaj( const char * );
                void pytanie( TPytanie & );
};

#endif
