#ifndef _TSTAN_H
#define _TSTAN_H

#include <string>
#include <deque>

class TStan {
        protected:
                string kat;
                deque< string > stany;
                string kateg;
        public:
                TStan();
                ~TStan();
                void zapisz( string , const string & );
                char bylo( string , const string & );
};
#endif
