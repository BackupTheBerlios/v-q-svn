//---------------------------------------------------------------------------
#ifndef dumpableH
#define dumpableH

#include <iostream>

using namespace std;
class Dumpable {
        public:
                virtual ostream & operator >> ( ostream & ) = 0;
};
//---------------------------------------------------------------------------

#endif
