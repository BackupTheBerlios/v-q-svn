#ifndef _BLAD_H
#define _BLAD_H

#include <string>

class Blad {
        string w;
        
    public:
        Blad() { w = "niezdefiniowany blad"; }
        Blad( string b) { w = b; };
        Blad( const char * b ) { w = b; };
        virtual const char * what() { return w.c_str(); };        
        virtual ~Blad() {}
};

#endif
