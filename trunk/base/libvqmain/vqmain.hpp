#ifndef __VQMAIN_HPP
#define __VQMAIN_HPP

#include <string>

/// from environment variable VQ_HOME, default /var/vq
extern std::string VQ_HOME;

int vqmain( int, char ** );

#endif // ifndef __VQMAIN_HPP
