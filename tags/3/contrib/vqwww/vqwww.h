#ifndef __VQWWW_H
#define __VQWWW_H

#include <deque>
#include <vector>
#include <cgicc/Cgicc.h>
#include <iostream>

#include "cmod.h"
#include "csess.h"
#include "cvq.h"

using namespace cgicc;

/// cgi
extern Cgicc *cgi;
/// end of variables marker
extern const_form_iterator cgie;
/// environment
extern const CgiEnvironment *ce;
/// modules in the system
extern std::deque<cmod*> mods;
/// cookies to set
extern std::vector<HTTPCookie> cks_set;
/// currently set cookies
extern const std::vector<HTTPCookie> *cks_cur;
/// output streams: normal, error
extern std::ostream *o, *e; 
/// request method
extern enum _reqmet { get, post } reqmet;
/// variables stored in session
extern struct _sessv {
        cvq::auth_info ai;
        std::string cip;
		std::string locale;
} sessv;
/// session
extern csess *sess;
/// vq 
extern cvq *vq;

/// module's information
typedef struct {
    void (*mods_init)( std::deque< cmod * > & );
} mod_info;

/// vqwww directories
extern struct _vqwww_dirs {
    std::string base, conf, share, libexec, include;
} vqwww_dirs;

#endif
