#ifndef _TWYNIKI_H
#define _TWYNIKI_H

#include <vector>
#include <string>

class TWynik {
        public:
                unsigned ile;
                string kto, kiedy;
                TWynik() { ustaw_kiedy(); }
                void ustaw_kiedy();
};

class TWyniki {
        protected:
               unsigned ile;
               TWynik wyn;
        public:
               TWyniki( unsigned i = 10 ) { ile = i; }
               void pobierz( vector< TWynik > & );
               void pobierz( fstream &, vector < TWynik > & );
               void dodaj( const string &, unsigned );
               char sprawdz( unsigned );
};

#endif
