// File based streams -*- C++ -*-

// Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003
// Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

//
// ISO C++ 14882: 27.8  File-based streams
//

/** @file pfstream
 *  This is a Standard C++ Library header.  You should @c #include this header
 *  in your programs, rather than any of the "st[dl]_*.h" implementation files.
 */

#ifndef _CPP_PFSTREAM
#define _CPP_PFSTREAM	1

#pragma GCC system_header

#include <istream>
#include <ostream>
#include <locale>	// For codecvt
#include <bits/basic_file.h>
#include <bits/gthr.h>
#include <ext/stdio_filebuf.h> // For stdio_filebuf which has rdbuf()->fd()

namespace fs
{
  using __gnu_cxx::stdio_filebuf;
  using namespace std;

  // 27.8.1.5  Template class basic_ipfstream
  /**
   *  Derivation of general input streams, specific to files.
  */
  template<typename _CharT, typename _Traits>
    class basic_ipfstream : public basic_istream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef stdio_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_istream<char_type, traits_type>	__istream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
     // Constructors/Destructors:
     /** Default constructor.  Create an input file stream.  */
      basic_ipfstream()
      : __istream_type(NULL), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief Create an input file stream.
       *  @param  s  Null terminated string specifying filename.
       *  @param  mode  Open file in specified mode (see std::ios_base).
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
      */
      explicit
      basic_ipfstream(const char* __s, ios_base::openmode __mode = ios_base::in)
      : __istream_type(NULL), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

      ~basic_ipfstream()
      { }

      // Members:
      /**
       *  @brief  Get a pointer to the file stream's buffer.
       *  @return Pointer to basic_filebuf.
      */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      bool
      is_open() { return _M_filebuf.is_open(); }

      void
      open(const char* __s, ios_base::openmode __mode = ios_base::in)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::in))
	  this->setstate(ios_base::failbit);
      }

      /** Close the file.  */
      void
      close()
      {
	if (!_M_filebuf.close())
	  this->setstate(ios_base::failbit);
      }
    };


  // 27.8.1.8  Template class basic_opfstream
  /**
   *  Derivation of general output streams, specific to files.
  */
  template<typename _CharT, typename _Traits>
    class basic_opfstream : public basic_ostream<_CharT,_Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef stdio_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_ostream<char_type, traits_type>	__ostream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
      // Constructors:
      /** Default constructor for output file_stream.  */
      basic_opfstream()
      : __ostream_type(NULL), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief  Create an output stream.
       *  @param  s  Null terminated string specifying filename.
       *  @param  mode  Open file in specified mode (see std::ios_base).
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
      */
      explicit
      basic_opfstream(const char* __s,
		     ios_base::openmode __mode = ios_base::out|ios_base::trunc)
      : __ostream_type(NULL), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

      ~basic_opfstream()
      { }

      // Members:
      /**
       *  @brief  Get a pointer to the file stream's buffer.
       *  @return Pointer to basic_filebuf.
      */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief Query to see if file stream is open.
       *  @return True if stream is open.
      */
      bool
      is_open() { return _M_filebuf.is_open(); }

      /**
       *  @brief Specify a file to open for output.
       *  @param  s  Null terminated string specifying filename.
       *  @param  mode  Mode in which to open file (see std::ios_base).
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
      */
      void
      open(const char* __s,
	   ios_base::openmode __mode = ios_base::out | ios_base::trunc)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::out))
	  this->setstate(ios_base::failbit);
      }

      /** Close the file stream.  */
      void
      close()
      {
	if (!_M_filebuf.close())
	  this->setstate(ios_base::failbit);
      }
    };


  // 27.8.1.11  Template class basic_pfstream
  /**
   *  Derivation of general input/output streams, specific to files.
  */
  template<typename _CharT, typename _Traits>
    class basic_pfstream : public basic_iostream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef stdio_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_ios<char_type, traits_type>		__ios_type;
      typedef basic_iostream<char_type, traits_type>	__iostream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
      // Constructors/destructor:
      /** Default constructor.  Create a file stream.  */
      basic_pfstream()
      : __iostream_type(NULL), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief Create an input/output stream.
       *  @param  s  Null terminated string specifying filename.
       *  @param  mode  Open file in specified mode (see std::ios_base).
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
      */
      explicit
      basic_pfstream(const char* __s,
		    ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __iostream_type(NULL), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

      ~basic_pfstream()
      { }

      // Members:
      /**
       *  @brief  Get a pointer to the file stream's buffer.
       *  @return Pointer to basic_filebuf.
      */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief Query to see if file stream is open.
       *  @return True if stream is open.
      */
      bool
      is_open() { return _M_filebuf.is_open(); }

      /**
       *  @brief Specify a file to open for input and/or output.
       *  @param  s  Null terminated string specifying filename.
       *  @param  mode  Mode in which to open file (see std::ios_base).
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
      */
      void
      open(const char* __s,
	   ios_base::openmode __mode = ios_base::in | ios_base::out)
      {
	if (!_M_filebuf.open(__s, __mode))
	  setstate(ios_base::failbit);
      }

      /** Close the file stream.  */
      void
      close()
      {
	if (!_M_filebuf.close())
	  setstate(ios_base::failbit);
      }
    };
  
  template<typename _CharT, typename _Traits = char_traits<_CharT> >
              class basic_ipfstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
             class basic_opfstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
              class basic_pfstream;

  typedef basic_ipfstream<char>      ipfstream;
  typedef basic_opfstream<char>      opfstream;
  typedef basic_pfstream<char>       pfstream;
}
 // namespace fs

#endif
