#ifndef __CREDIRMOD_H
#define __CREDIRMOD_H

#include <string>
#include <functional>

#include "cvq.h"
#include "cmod.h"

class credirmod : public cmod {
        protected:
                static const char _id[];
                static const char _conf[];
                static const char _name[];

                void act();

                struct _cmp : public std::unary_function<const cvq::udot_info &, bool> {
                        std::string id;
                        result_type operator ()(argument_type ptr) {
                                return ptr.id == id ? true : false;
                        }
                } cmp;

                void rmspace(std::string &);
        public:
                credirmod() { modinfo=mi_conf; }
                const char *id() { return _id; }
                const char *name() { return _name; }
                const char *menu() { return ""; }
                const char *conf() { return _conf; }
                char run();
                virtual ~credirmod() {}
};

#endif
