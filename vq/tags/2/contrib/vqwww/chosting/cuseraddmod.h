#ifndef __CUSERADDMOD_H
#define __CUSERADDMOD_H

#include <string>
#include <memory>

#include "cmod.h"
#include "cinfodb.h"

class cuseraddmod : public cmod {
        protected:
                std::auto_ptr<cinfodb> idb;
                static const char _id[];
                static const char _conf[];
                static const char _name[];

                void act();
                void notify_run(const std::string &);
        public:
                cuseraddmod();
                virtual ~cuseraddmod() {}
                const char *id() { return _id; }
                const char *name() { return _name; }
                const char *menu() { return ""; }
                const char *conf();
                char run();
};

#endif
