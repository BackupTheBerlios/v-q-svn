//  (C) Copyright Beman Dawes 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

/*
 * Copyright (c) 1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 * 2003-07-28, Code modified by Pawel Niewiadomski
 */ 
//----------------------------------------------------------------------------// 

#ifndef __PFSTREAM_H
#define __PFSTREAM_H

#include <iosfwd>
#include <fstream>
#if __GNUC__ < 3
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

namespace posix
{
#if __GNUC__ >= 3 

template < class charT, class traits = std::char_traits<charT> >
class basic_filebuf : public std::basic_filebuf<charT,traits> {
public:
		typedef std::basic_filebuf<charT, traits> __filebuf_type;
		typedef typename std::basic_filebuf<charT, traits>::int_type int_type;
		typedef typename std::basic_filebuf<charT, traits>::char_type char_type;

protected:
		char_type _M_unbuf[4];

public:
		int fd() {
				return _M_file.fd();
		}

		__filebuf_type* open( int, std::ios_base::openmode , 
				bool __del = true, int_type __size = 0 );

		__filebuf_type* open( const charT * __s, std::ios_base::openmode __om ) {
				return std::basic_filebuf<charT, traits>::open(__s, __om);
		}
};

template <typename charT, typename traits>
	typename basic_filebuf<charT, traits>::__filebuf_type* 
	basic_filebuf<charT, traits>::
	open( int __fd, std::ios_base::openmode __mode, 
			bool __del, typename basic_filebuf<charT, traits>::int_type __size ) {
		__filebuf_type *__ret = NULL;
		
		_M_file.sys_open(__fd, __mode, __del);
		if (this->is_open()) {
				_M_mode = __mode;
				if (__size > 0 && __size < 4) {
						// Specify unbuffered.
						_M_buf = _M_unbuf;
						_M_buf_size = __size;
						_M_buf_size_opt = 0;
				} else {
						_M_buf_size_opt = __size;
						_M_allocate_internal_buffer();
				}
				_M_set_indeterminate();
				__ret = this;
		}
		return __ret;
}

//----------------------------------------------------------------------
// Class basic_ipfstream<>

template <class _CharT, class _Traits = std::char_traits<_CharT> >
class basic_ipfstream : public std::basic_istream<_CharT, _Traits>
{
public:                         // Types
  typedef _CharT                     char_type;
  typedef typename _Traits::int_type int_type;
  typedef typename _Traits::pos_type pos_type;
  typedef typename _Traits::off_type off_type;
  typedef _Traits                    traits_type;

  typedef std::basic_ios<_CharT, _Traits>                _Basic_ios;
  typedef std::basic_istream<_CharT, _Traits>            _Base;
  typedef basic_filebuf<_CharT, _Traits>            _Buf;

public:                         // Constructors, destructor.

  basic_ipfstream() : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_istream<_CharT, _Traits>(0), _M_buf() {
      this->init(&_M_buf);
  }

  explicit basic_ipfstream(const char* __s, ios_base::openmode __mod = ios_base::in) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_istream<_CharT, _Traits>(0),
    _M_buf() {
      this->init(&_M_buf);
      if (!_M_buf.open(__s, __mod | ios_base::in))
	this->setstate(ios_base::failbit);
  }

# ifndef _STLP_NO_EXTENSIONS
  explicit basic_ipfstream(int __id, ios_base::openmode __mod = ios_base::in) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_istream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__id, __mod | ios_base::in))
      this->setstate(ios_base::failbit);
  }
  basic_ipfstream(const char* __s, ios_base::openmode __m,
		 long __protection) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_istream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__s, __m | ios_base::in, __protection))
      this->setstate(ios_base::failbit);  
  }
  
# endif

  ~basic_ipfstream() {}

public:                         // File and buffer operations.
  _Buf * rdbuf() const
    { return const_cast<_Buf*>(&_M_buf); }

  bool is_open() {
    return this->rdbuf()->is_open();
  }

  void open(const char* __s, ios_base::openmode __mod = ios_base::in) {
    if (!this->rdbuf()->open(__s, __mod | ios_base::in))
      this->setstate(ios_base::failbit);
  }

  void close() {
    if (!this->rdbuf()->close())
      this->setstate(ios_base::failbit);
  }

  int fd() {
  	return _M_buf.fd();
  }

private:
  _Buf _M_buf;
};


//----------------------------------------------------------------------
// Class basic_opfstream<>

template <class _CharT, class _Traits = std::char_traits<_CharT> >
class basic_opfstream : public std::basic_ostream<_CharT, _Traits>
{
public:                         // Types
  typedef _CharT                     char_type;
  typedef typename _Traits::int_type int_type;
  typedef typename _Traits::pos_type pos_type;
  typedef typename _Traits::off_type off_type;
  typedef _Traits                    traits_type;

  typedef std::basic_ios<_CharT, _Traits>                _Basic_ios;
  typedef std::basic_ostream<_CharT, _Traits>            _Base;
  typedef basic_filebuf<_CharT, _Traits>            _Buf;

public:                         // Constructors, destructor.
  basic_opfstream() : 
    std::basic_ios<_CharT, _Traits>(), 
    std::basic_ostream<_CharT, _Traits>(0), _M_buf() {
      this->init(&_M_buf);
  }
  explicit basic_opfstream(const char* __s, ios_base::openmode __mod = ios_base::out) 
    : std::basic_ios<_CharT, _Traits>(), std::basic_ostream<_CharT, _Traits>(0),
      _M_buf() {
	this->init(&_M_buf);
	if (!_M_buf.open(__s, __mod | ios_base::out))
	  this->setstate(ios_base::failbit);
  }

# ifndef _STLP_NO_EXTENSIONS
  explicit basic_opfstream(int __id, ios_base::openmode __mod = ios_base::out) 
    : std::basic_ios<_CharT, _Traits>(), std::basic_ostream<_CharT, _Traits>(0),
    _M_buf() {
 	this->init(&_M_buf);
 	if (!_M_buf.open(__id, __mod | ios_base::out))
 	  this->setstate(ios_base::failbit);
  }
  basic_opfstream(const char* __s, ios_base::openmode __m, long __protection) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_ostream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__s, __m | ios_base::out, __protection))
      this->setstate(ios_base::failbit);  
  }
# endif
  
  ~basic_opfstream() {}

public:                         // File and buffer operations.
  _Buf * rdbuf() const
    { return const_cast<_Buf*>(&_M_buf); } 

  bool is_open() {
    return this->rdbuf()->is_open();
  }

  void open(const char* __s, ios_base::openmode __mod= ios_base::out) {
    if (!this->rdbuf()->open(__s, __mod | ios_base::out))
      this->setstate(ios_base::failbit);
  }

  void close() {
    if (!this->rdbuf()->close())
      this->setstate(ios_base::failbit);
  }

  int fd() {
  	return _M_buf.fd();
  }

private:
  _Buf _M_buf;
};


//----------------------------------------------------------------------
// Class basic_pfstream<>

template <class _CharT, class _Traits = std::char_traits<_CharT> >
class basic_pfstream : public std::basic_iostream<_CharT, _Traits>
{
public:                         // Types
  typedef _CharT                     char_type;
  typedef typename _Traits::int_type int_type;
  typedef typename _Traits::pos_type pos_type;
  typedef typename _Traits::off_type off_type;
  typedef _Traits                    traits_type;

  typedef std::basic_ios<_CharT, _Traits>                _Basic_ios;
  typedef std::basic_iostream<_CharT, _Traits>           _Base;
  typedef basic_filebuf<_CharT, _Traits>            _Buf;

public:                         // Constructors, destructor.
  
  basic_pfstream()   
    : std::basic_ios<_CharT, _Traits>(), std::basic_iostream<_CharT, _Traits>(0), _M_buf() {
      this->init(&_M_buf);
  }

  explicit basic_pfstream(const char* __s,
                         ios_base::openmode __mod = ios_base::in | ios_base::out) :
    std::basic_ios<_CharT, _Traits>(), std::basic_iostream<_CharT, _Traits>(0), _M_buf() {
      this->init(&_M_buf);
      if (!_M_buf.open(__s, __mod))
	this->setstate(ios_base::failbit);
  }
  
# ifndef _STLP_NO_EXTENSIONS
  explicit basic_pfstream(int __id,
                         ios_base::openmode __mod = ios_base::in | ios_base::out) :
    std::basic_ios<_CharT, _Traits>(), std::basic_iostream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__id, __mod))
      this->setstate(ios_base::failbit);
  }
  basic_pfstream(const char* __s, ios_base::openmode __m, long __protection) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_iostream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__s, __m, __protection))
      this->setstate(ios_base::failbit);  
  }
# endif    
  ~basic_pfstream() {}

public:                         // File and buffer operations.

  _Buf * rdbuf() const
    { return const_cast<_Buf*>(&_M_buf); } 

  bool is_open() {
    return this->rdbuf()->is_open();
  }

  void open(const char* __s, 
	    ios_base::openmode __mod = 
	    ios_base::in | ios_base::out) {
    if (!this->rdbuf()->open(__s, __mod))
      this->setstate(ios_base::failbit);
  }
  
  void close() {
    if (!this->rdbuf()->close())
      this->setstate(ios_base::failbit);
  }

  int fd() {
  	return _M_buf.fd();
  }
  
private:
  _Buf _M_buf;
};

typedef basic_pfstream<char> pfstream;
typedef basic_ipfstream<char> ipfstream;
typedef basic_opfstream<char> opfstream;

#else

class pfstream : public fstream {
		public:
				pfstream() {}
				pfstream(int fd) : fstream(fd) {}
				pfstream(const char *fn, int mode, int prot=0644) {
					open(fn, mode, prot);
				}
				void open(const char *fn, int mode, int prot=0644) {
					struct stat st;
					if( (mode & in) && stat(fn, &st) ) {
							setstate(badbit);
							return;
					}
					fstream::open(fn, mode, prot);
				}
};
		
class ipfstream : public ifstream {
		public:
				ipfstream() {}
				ipfstream(int fd) : ifstream(fd) {}
				ipfstream(const char *fn, int mode = in, int prot=0644) {
					open(fn, mode, prot);
				}
				void open(const char *fn, int mode = in, int prot=0644) {
					struct stat st;
					if( (mode & in) && stat(fn, &st) ) {
							setstate(badbit);
							return;
					}
					ifstream::open(fn, mode, prot);
				}
};

class opfstream : public ofstream {
		public:
				opfstream() {}
				opfstream(int fd) : ofstream(fd) {}
				opfstream(const char *fn, int mode = out, int prot=0644) {
					open(fn, mode, prot);
				}
				void open(const char *fn, int mode = out, int prot=0644) {
					struct stat st;
					if( (mode & in) && stat(fn, &st) ) {
							setstate(badbit);
							return;
					}
					ofstream::open(fn, mode, prot);
				}

};

#endif
} // namespace boost

#endif  // __PFSTREAM_H

