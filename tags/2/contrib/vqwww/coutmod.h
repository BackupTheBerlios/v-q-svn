#ifndef __COUTMOD_H
#define __COUTMOD_H

#include "cmod.h"

class coutmod : public cmod {
        private:
                static const char _id[];
        public:
                coutmod() { modinfo=mi_menu; }
                const char *id() { return _id; }
                const char *name() { return ""; }
                const char *menu() { return "<out/>"; }
                const char *conf() { return ""; }
                char run();
};

#endif
