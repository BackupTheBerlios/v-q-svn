/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "main.hpp"
#include "text.hpp"
#include "sys.hpp"
#include "cautoresp.hpp"

#include <string>
#include <map>
#include <utility>
#include <list>
#include <iostream>
#include <stdexcept>
#include <cctype>

using namespace std;
using namespace text;
using namespace sys;

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
int cppmain(int ac , char **av) {
	try {
			if( ac < 3 || ac > 4 ) {
   					usage(*av);
   					return 111;
 			}

 			autoresp = cautoresp::alloc();
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
