/*
Copyright (c) 2003 Pawel Niewiadomski
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the Pawel Niewiadomski,
   and other contributors.
4. Neither the name of Pawel Niewiadomski nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PAWEL NIEWIADOMSKI AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL PAWEL NIEWIADOMSKI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#ifndef __CUIMOD_H
#define __CUIMOD_H

#include <string>
#include <vector>
#include <memory>
#include <cmod.h>
#include "cdbdaemon.h"

class cuimod : public cmod {
        protected:
                std::string locale; // localization information (for example pl_PL)
                std::vector<cdbdaemon::edu_info> edu_levs;
                std::vector<cdbdaemon::work_info> works;
                std::vector<cdbdaemon::int_info> ints;
                std::vector<cdbdaemon::area_info> areas;
                std::vector<cdbdaemon::sex_info> sexes;
                cdbdaemon::user_info ui, ui_org;
                cdbdaemon::domain_info dominfo;
                
                std::auto_ptr<cdbdaemon> idb;
                static const char _id[];
                static const char _name[];
                static const char _conf[];

                void act();
                void ints2form();
                void form2ints();
                void post_act();
        public:
                cuimod();
                const char *id() { return _id; }
                const char *name() { return _name; }
                const char *menu() { return ""; }
                const char *conf() { return _conf; }
                char run();
                virtual ~cuimod() {}
};

#endif
