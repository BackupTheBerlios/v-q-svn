#ifndef __HMAC_MD5
#define __HMAC_MD5

/* prototypes */

void hmac_md5( const unsigned char* , 
		int , const unsigned char* , int , unsigned char* );

/* pointer to data stream */
/* length of data stream */
/* pointer to authentication key */
/* length of authentication key */
/* caller digest to be filled in */

#endif
