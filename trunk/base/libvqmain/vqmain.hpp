#ifndef __VQMAIN_HPP
#define __VQMAIN_HPP

#include <string>

/// from environment variable VQ_HOME or -H arg, default /var/vq
extern std::string VQ_HOME;
/// from environment variable VQ_ETC_DIR or -E arg, default VQ_HOME/etc
export std::string VQ_ETC_DIR;

int vqmain( int, char ** );

#endif // ifndef __VQMAIN_HPP
