#ifndef __CPASSMOD_H
#define __CPASSMOD_H

#include "cmod.h"

class cpassmod : public cmod {
        protected:
                static const char _id[];
                static const char _conf[];
                static const char _name[];

                void act();
        public:
                cpassmod() { modinfo=mi_conf; }
                const char *id() { return _id; }
                const char *name() { return _name; }
                const char *menu() { return ""; }
                const char *conf() { return _conf; }
                char run();
};

#endif
