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

#ifndef __CMAILDIR_H
#define __CMAILDIR_H

/**
 * Maildir++
 */
class cmaildir { 
		public:
			/**
			 * \defgroup state Maildir state
			 */
			//@{
				typedef int iostate;

				/// Some error occured
				const static iostate badbit = 1;
				/// Action failed
				const static iostate failbit = 2;
				/// All ok
				const static iostate goodbit = 0;
			//@}
				
				cmaildir(const char *);
				void open(const char *);
				void close();

			/**
			 * \ingroup state
			 */
			//@{
				inline iostate rdstate() const;
				inline void clear( iostate = goodbit );
				inline void setstate( iostate );
				inline bool bad() const;
				inline bool good() const;
				inline bool fail() const;
			//@}
				

		protected:
			/**
			 * \ingroup state
			 */
			//@{
				iostate state; //!< state
			//@}
			
};

bool cmaildir::fail() const {
	return (rdstate() & (badbit | failbit)) != 0;
}

bool cmaildir::bad() const {
	return (rdstate() & badbit) != 0;
}

bool cmaildir::good() const {
	return ! rdstate();
}

void cmaildir::setstate( iostate is ) {
	clear(rdstate() | is);
}

void cmaildir::clear( iostate is ) {
	state = is;
}

#endif // ifndef __CMAILDIR_H
