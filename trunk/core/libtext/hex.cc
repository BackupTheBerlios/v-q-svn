/*
Copyright (c) 2002,2003,2004 Pawel Niewiadomski
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

#include "hex.hpp"

namespace text {

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

} // namespace text

