/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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


#ifndef __CREDIRMOD_H
#define __CREDIRMOD_H

#include <string>
#include <functional>

#include "cvq.h"
#include "cmod.h"

/**
 * \ingroup mod
 */
//@{

/**
 * Setup e-mail redirections.
 */
class credirmod : public cmod {
        protected:
                static const char _id[];
                static const char _conf[];

                void act();
				void list( std::vector<cvq::udot_info> & );
				void post_read( std::vector<cvq::udot_info>::size_type & cnt,
						std::vector<cvq::udot_info> &, 
						std::vector<cvq::udot_info> &, 
						std::vector<cvq::udot_info> & );

				void post( std::vector<cvq::udot_info> &,
						std::vector<cvq::udot_info>::size_type & );
				
				/// Comparator for find
                struct _cmp : public std::unary_function<const cvq::udot_info &, bool> {
                        std::string id;
                        result_type operator ()(argument_type ptr) {
                                return ptr.id == id ? true : false;
                        }
                } cmp;

                void rmspace(std::string &);
        public:
                credirmod( cenvironment & e ) : cmod(e) { modinfo=mi_conf; }
                const char *id() { return _id; }
                const char *name() { return ""; }
                const char *menu() { return ""; }
                const char *conf() { return _conf; }
                char run();
                virtual ~credirmod() {}
};
//@}
#endif
