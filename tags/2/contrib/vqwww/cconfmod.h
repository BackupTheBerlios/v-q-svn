#ifndef __CCONFMOD_H
#define __CCONFMOD_H

#include "cmod.h"

class cconfmod : public cmod {
        protected:
                static const char _id[];
                static const char _menu[];
                static const char _name[];
        public:
                cconfmod() { modinfo=mi_menu; }
                const char *id() { return _id; }
                const char *name() { return _name; }
                const char *menu() { return _menu; }
                const char *conf() { return ""; }
                char run();
};

#endif
