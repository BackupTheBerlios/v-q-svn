#ifndef __CUSERADDMOD_H
#define __CUSERADDMOD_H

#include <string>
#include <memory>

#include "cmod.h"
#include "cinfodb.h"

/**
 * \ingroup mod
 */
//@{

/**
 * Module for adding users.
 */
class cuseraddmod : public cmod {
        protected:
                std::auto_ptr<cinfodb> idb;
                static const char _id[];
                static const char _conf[];

                void act();
                void notify_run(const std::string &);
        public:
                cuseraddmod( cenvironment &  );
                virtual ~cuseraddmod() {}
                const char *id() { return _id; }
				//@{
				/// not used
                const char *name() { return ""; }
                const char *menu() { return ""; }
				//@}
                const char *conf();
                char run();
};
//@}
#endif
