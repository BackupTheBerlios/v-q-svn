/*
Copyright (c) 2003,2004 Pawel Niewiadomski
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

#ifndef __CCONF_H
#define __CCONF_H

#include <string>
#include <stdexcept>
#include <map>
#include <inttypes.h>

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

#endif
