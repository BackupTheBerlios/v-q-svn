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

#include <string>
#include <map>
#include <utility>
#include <list>
#include <iostream>
#include <stdexcept>
#include <cctype>

#include "lower.h"
#include "sys.h"
#include "cautoresp.h"

using namespace std;

cautoresp *autoresp = NULL; 

void usage(const char *me) {
	cout<<"usage: "<<me<<" message [headers]"<<endl
		<<endl
		<<"Program generates reply on incoming message. It should"<<endl
		<<"be executed by qmail-local (man dot-qmail)."<<endl
		<<"It takes two or three arguments - code page name, hexadecimal "<<endl
		<<"representation of message which will be send as a"<<endl
		<<"response; second argument can be hexadecimal representation of "<<endl
		<<"headers which should be placed in new message (some of them are"<<endl
		<<"overwritten with values from original message)."<<endl;
}

/**
 * Parses array of header fields into map (header name->value). Can process
 * any storage container.
 */
template <class T> void hdr_array2map( const T & array, 
		cautoresp::map_hdr_val & map ) {

	// insert values into map (map header name to its value)
	pair<cautoresp::map_hdr_val::key_type,cautoresp::map_hdr_val::data_type> 
			hdr_val_item;

	string::size_type sep_pos, field_len;
 	for( typename T::const_iterator field=array.begin(), fend=array.end(); 
			field!=fend; ++field ) {

		sep_pos = field->find(':');
		if(sep_pos == string::npos) continue; // ignore incorrect lines
		hdr_val_item.first = lower(field->substr(0, sep_pos));

		field_len = field->length();
		if( field_len > sep_pos ) {
				if( field_len > sep_pos+1
					&& isspace((*field)[sep_pos+1]) )
						hdr_val_item.second = field->substr(sep_pos+2);
				else hdr_val_item.second = field->substr(sep_pos+1);
		} else hdr_val_item.second = "";
		map[hdr_val_item.first] = hdr_val_item.second;// last takes precedence
	}
}

/**
 * \throw runtime_error on any error
 */
cautoresp::map_hdr_val input_read() {
	cautoresp::map_hdr_val hdr_val;
 	list<string> hdrs;
 	string ln;
 	
	// read in headers
 	while(getline(cin, ln)) {
   			if(ln.empty()) break;
   			if( isspace(ln[0]) ) hdrs.back() += ln;
   			else hdrs.push_back(ln);
 	}
 	if(cin.bad()) throw runtime_error("error while reading input");

	hdr_array2map< list<string> >( hdrs, hdr_val );
	return hdr_val;
}

/**
 * autoresp takes 1 argument. It's hexadecimal representation of:
 * hexadecimal representation of message, 0x00, hexadecimal
 * representation of headers to add. It may contain only
 * message double coded.
 */
int main(int ac , char **av) {
	try {
			if( ac < 3 || ac > 4 ) {
   					usage(*av);
   					return 111;
 			}

 			autoresp = cautoresp::obj_new();
			autoresp->codepage_set(av[1]);
			autoresp->msg_set(hex_from(av[2]));
			
			if( ac == 4 ) {
					list<string> hdrs;
					cautoresp::map_hdr_val add_hdr_val;
					string str = hex_from(av[3]);
					string::size_type beg=0, pos;
					for(; (pos=str.find('\n', beg)) != string::npos; beg = pos+1) {
							hdrs.push_back(str.substr(beg, pos-beg));
					}
					hdrs.push_back(str.substr(beg));
					hdr_array2map< list<string> >(hdrs, add_hdr_val);
					autoresp->hdrs_add_set(add_hdr_val);
			}
			autoresp->reply(input_read());
			return 0; 
	} catch( const exception & e ) {
			cout<<"autoresp: exception: "<<e.what()<<endl;
			return 111;
	}
}
