/*
Copyright (c) 2004 Pawel Niewiadomski
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
