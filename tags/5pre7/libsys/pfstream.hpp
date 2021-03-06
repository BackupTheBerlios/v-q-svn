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

#ifndef VQ__PFSTREAM_H
#define VQ__PFSTREAM_H

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
				return this->_M_file.fd();
		}

		__filebuf_type* open( int, std::ios_base::openmode );

		__filebuf_type* open( const charT * __s, std::ios_base::openmode __om ) {
				return std::basic_filebuf<charT, traits>::open(__s, __om);
		}
};

template <typename charT, typename traits>
	typename basic_filebuf<charT, traits>::__filebuf_type* 
	basic_filebuf<charT, traits>::
	open( int __fd, std::ios_base::openmode __mode ) {

	__filebuf_type *__ret = NULL;
	if (!this->is_open()) 
	{
			this->_M_file.sys_open(__fd, __mode);
			if (this->is_open())
			{
					this->_M_allocate_internal_buffer();
					this->_M_mode = __mode;

					// Setup initial buffer to 'uncommitted' mode.
					this->_M_reading = false;
					this->_M_writing = false;
					this->_M_set_buffer(-1);

					// Reset to initial state.
					this->_M_state_last = this->_M_state_cur = this->_M_state_beg;

					// 27.8.1.3,4
					if ((__mode & std::ios_base::ate)
						&& this->seekoff(0, std::ios_base::end, __mode)
							== typename std::basic_filebuf<charT,traits>::pos_type(
								typename std::basic_filebuf<charT,traits>::off_type(-1)))
							this->close();
					else
							__ret = this;
			}
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

  explicit basic_ipfstream(const char* __s, std::ios_base::openmode __mod = std::ios_base::in) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_istream<_CharT, _Traits>(0),
    _M_buf() {
      this->init(&_M_buf);
      if (!_M_buf.open(__s, __mod | std::ios_base::in))
	this->setstate(std::ios_base::failbit);
  }

# ifndef _STLP_NO_EXTENSIONS
  explicit basic_ipfstream(int __id, std::ios_base::openmode __mod = std::ios_base::in) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_istream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__id, __mod | std::ios_base::in))
      this->setstate(std::ios_base::failbit);
  }
  basic_ipfstream(const char* __s, std::ios_base::openmode __m,
		 long __protection) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_istream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__s, __m | std::ios_base::in, __protection))
      this->setstate(std::ios_base::failbit);  
  }
  
# endif

  ~basic_ipfstream() {}

public:                         // File and buffer operations.
  _Buf * rdbuf() const
    { return const_cast<_Buf*>(&_M_buf); }

  bool is_open() {
    return this->rdbuf()->is_open();
  }

  void open(const char* __s, std::ios_base::openmode __mod = std::ios_base::in) {
    if (!this->rdbuf()->open(__s, __mod | std::ios_base::in))
      this->setstate(std::ios_base::failbit);
  }

  void close() {
    if (!this->rdbuf()->close())
      this->setstate(std::ios_base::failbit);
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
  explicit basic_opfstream(const char* __s, std::ios_base::openmode __mod = std::ios_base::out) 
    : std::basic_ios<_CharT, _Traits>(), std::basic_ostream<_CharT, _Traits>(0),
      _M_buf() {
	this->init(&_M_buf);
	if (!_M_buf.open(__s, __mod | std::ios_base::out))
	  this->setstate(std::ios_base::failbit);
  }

# ifndef _STLP_NO_EXTENSIONS
  explicit basic_opfstream(int __id, std::ios_base::openmode __mod = std::ios_base::out) 
    : std::basic_ios<_CharT, _Traits>(), std::basic_ostream<_CharT, _Traits>(0),
    _M_buf() {
 	this->init(&_M_buf);
 	if (!_M_buf.open(__id, __mod | std::ios_base::out))
 	  this->setstate(std::ios_base::failbit);
  }
  basic_opfstream(const char* __s, std::ios_base::openmode __m, long __protection) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_ostream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__s, __m | std::ios_base::out, __protection))
      this->setstate(std::ios_base::failbit);  
  }
# endif
  
  ~basic_opfstream() {}

public:                         // File and buffer operations.
  _Buf * rdbuf() const
    { return const_cast<_Buf*>(&_M_buf); } 

  bool is_open() {
    return this->rdbuf()->is_open();
  }

  void open(const char* __s, std::ios_base::openmode __mod= std::ios_base::out) {
    if (!this->rdbuf()->open(__s, __mod | std::ios_base::out))
      this->setstate(std::ios_base::failbit);
  }

  void close() {
    if (!this->rdbuf()->close())
      this->setstate(std::ios_base::failbit);
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
                         std::ios_base::openmode __mod = std::ios_base::in | std::ios_base::out) :
    std::basic_ios<_CharT, _Traits>(), std::basic_iostream<_CharT, _Traits>(0), _M_buf() {
      this->init(&_M_buf);
      if (!_M_buf.open(__s, __mod))
	this->setstate(std::ios_base::failbit);
  }
  
# ifndef _STLP_NO_EXTENSIONS
  explicit basic_pfstream(int __id,
                         std::ios_base::openmode __mod = std::ios_base::in | std::ios_base::out) :
    std::basic_ios<_CharT, _Traits>(), std::basic_iostream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__id, __mod))
      this->setstate(std::ios_base::failbit);
  }
  basic_pfstream(const char* __s, std::ios_base::openmode __m, long __protection) : 
    std::basic_ios<_CharT, _Traits>(),  std::basic_iostream<_CharT, _Traits>(0), _M_buf() {
    this->init(&_M_buf);
    if (!_M_buf.open(__s, __m, __protection))
      this->setstate(std::ios_base::failbit);  
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
	    std::ios_base::openmode __mod = 
	    std::ios_base::in | std::ios_base::out) {
    if (!this->rdbuf()->open(__s, __mod))
      this->setstate(std::ios_base::failbit);
  }
  
  void close() {
    if (!this->rdbuf()->close())
      this->setstate(std::ios_base::failbit);
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

