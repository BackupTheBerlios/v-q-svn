#ifndef __HMAC_MD5
#define __HMAC_MD5

#include "common.hpp"

namespace text {

	/* prototypes */

	LIBTEXT_API void hmac_md5( const unsigned char* , 
		int , const unsigned char* , int , unsigned char* );

	/* pointer to data stream */
	/* length of data stream */
	/* pointer to authentication key */
	/* length of authentication key */
	/* caller digest to be filled in */

	LIBTEXT_API inline void hmac_md5( const char* ds, 
		int dl, const char* ak, int al, char* dig ) {

		return hmac_md5( reinterpret_cast<const unsigned char *>(ds), dl,
			reinterpret_cast<const unsigned char *>(ak), al,
			reinterpret_cast<unsigned char *>(dig));
	}

} // namespace text

#endif // ifndef __HMAC_MD5
