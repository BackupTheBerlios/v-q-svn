/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new@foo-baz.com)

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

#ifndef __CCONF_HPP
#define __CCONF_HPP

#include <string>
#include <stdexcept>
#include <map>
#include <inttypes.h>

namespace conf {
	/**
	 * base class representing configuration file
	 */
	class cconf {
			public:
				/**
				 * \defgroup err Errors handling
				 */
				/**
				 * \defgroup exceptions Exceptions
				 * \ingroup err
				 */
				//@{
					/**
					 * error in cconf
					 */
					class err : public std::exception {
							protected:
									std::string w; //!< error message
							public:
									/// Cons.
									err(const cconf *cc, const std::string &s)
											: w(cc->fn) {
										w+=": ";
										w+=s;
									}
									/// Cons.
									err(const cconf *cc, const std::string &s, 
											const std::string &s1)
											: w(cc->fn) {
										w+=": ";
										w+=s;
										w+=s1;
									}
	
									/// Destructor
									virtual ~err() throw () {}
									
									/// Returns error message
									const char *what() const throw() {
										return w.c_str();
									}
					};
			
					/**
					 * can't read
					 */
					class read_error : public err {
							public:
									/// Cons.
									read_error(const cconf * cc,
											const std::string &w)
											: err(cc, "can't read: ", w) {}
									/// Destructor
									virtual ~read_error() throw() {}
					};
	
					/**
					 * value can't be empty
					 */
					class empty_error : public err {
							public:
									/// Cons.
									empty_error(const cconf * cc)
											: err(cc, "value can't be empty") {}
									/// Destructor
									virtual ~empty_error() throw() {}
					};
				
					/**
					 * value not in specified range
					 */
					class range_error : public err {
							public:
									/// Cons.
									range_error(const cconf *cc,
											const std::string &v,
											const std::string &min,
											const std::string &max)
											: err(cc, "value not in range: ") {
										w+=v;
										w+="; minimum is: ";
										w+=min;
										w+="; maximum is: ";
										w+=max;
									}
									/// Destructor
									virtual ~range_error() throw() {}
					};
				//@}
			
				/**
				 * \ingroup err
				 */
				//@{
					friend class err; //!< Exceptions can read state of cconf object
				//@}
	/**************************/
			protected:
	
					std::string fn; //!< file name
					bool read; //!< did we read it from a file?
	
					/// Can't create cconf object
					cconf(const std::string &fn) : fn(fn), read(false) {}
	
	};
	
	/**
	 * read first line from file
	 */
	class clnconf : public cconf {
			public:
					clnconf(const std::string &fn, const std::string & def);
					virtual std::string val_str(); //!< return configured value
					virtual ~clnconf() {}
			protected:
					std::string val; //!< line read from configuration file
					std::string ln_read(); //!< used to read from file
	};
	
	/**
	 * read integer value from first line of file, it can read decimal or octal
	 */
	class cintconf : public clnconf {
			public:
					cintconf(const std::string &fn, const std::string &def,
							int32_t = 1 );
					cintconf(const std::string &fn, const std::string &def,
							int32_t, int32_t, int32_t = 1 );
	
					virtual int32_t val_int(); //!< return value as int
					std::string val_str(); //!< return value as string
					virtual ~cintconf() {}
	
			protected:
					int32_t ival; //!< current value
					int32_t min; //!< minimal value
					int32_t max; //!< maximal value
					int32_t mult; //!< multiplier
	
					int32_t str2val( const std::string & ); //!< covert string to value
	
	};
	
	/**
	 * get uid which owns file (by fstat)
	 */
	class cuidconf : public cintconf {
			public:
					cuidconf(const std::string &fn, const std::string &def);
					int32_t val_int();
	};
	
	/**
	 * get gid which owns file (by fstat)
	 */
	class cgidconf : public cintconf {
			public:
					cgidconf(const std::string &fn, const std::string &def);
					int32_t val_int();
	};
	
	/**
	 * Configuration file containing simple map, each line looks like following: 
	 * key:value
	 */
	class cmapconf : public cconf {
			public:
					/// this type holds information read from file
					typedef std::map<std::string, std::string> map_type;
	
					cmapconf(const std::string &fn);
					const map_type & val_map(); //!< give access to read configuration
	
			protected:
					map_type map; //!< configuration from file
	
	
	};
} // namespace conf

#endif // ifndef __CCONF_HPP
