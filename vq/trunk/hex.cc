#include "sys.h"

using namespace std;

/*
 */
string to_hex(const unsigned char *buf, unsigned bufl)
{
	static const unsigned char hex[] = "0123456789abcdef";
	string hexbuf;
	hexbuf.reserve(bufl<<1);
	for( unsigned i=0; i < bufl; i++ ) {
			hexbuf += hex[ (buf[i] & 0xf0) >> 4 ];
			hexbuf += hex[ buf[i] & 0xf ]; 
	}
	return hexbuf;
}

/**
 * 
 */
string hex_from( const string & hex ) {
	string bin;
	string::size_type hexl=hex.length();
	string::const_iterator hex_cur=hex.begin(), hex_end = hex.end();
	char val, byte=0;

	if(!hexl) return bin;
#define hex_char(x) \
	switch(x) { \
	case '0': val = 0; break; \
	case '1': val = 1; break; \
	case '2': val = 2; break; \
	case '3': val = 3; break; \
	case '4': val = 4; break; \
	case '5': val = 5; break; \
	case '6': val = 6; break; \
	case '7': val = 7; break; \
	case '8': val = 8; break; \
	case '9': val = 9; break; \
	case 'a': case 'A': val = 10; break; \
	case 'b': case 'B': val = 11; break; \
	case 'c': case 'C': val = 12; break; \
	case 'd': case 'D': val = 13; break; \
	case 'e': case 'E': val = 14; break; \
	case 'f': case 'F': val = 15; break; \
	default: \
		return bin; \
	}

	if( hexl % 2 ) {
		hex_char(*hex_cur);
		bin += val;
		++hex_cur;
	}
	for( ; hex_cur!=hex_end; ++ hex_cur ) {
			hex_char(*hex_cur);
			byte = val<<4;
			++hex_cur;
			hex_char(*hex_cur);
			byte = byte | val;
			bin += byte;
	}
	return bin;
}
