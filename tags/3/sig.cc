/*
Copyright (c) 2002,2003 Pawel Niewiadomski
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

#include <csignal>
#include <cstdlib>

#include "sig.h"

/**
set SIGCHLD flag to SA_NOCLDSTOP (tell that we don't want our children
return info)
@return true ok, false error
*/
bool sig_chld_nocldwait()
{
	struct sigaction sa;
	sigemptyset( & sa.sa_mask );
	sa.sa_flags= SA_NOCLDWAIT;
	sa.sa_handler = SIG_DFL;
	if( sigaction( SIGCHLD, & sa, NULL ) )
			return false;
	return true;
}

/**
set handler for SIGPIPE 
@return true on success
*/
bool sig_pipe( void (*f)(int) )
{
	return sig_catch( SIGPIPE, f );
}

/**
*/
bool sig_term( void (*f)(int) )
{
	return sig_catch( SIGTERM, f );
}
/*
*/
bool sig_int( void (*f)(int) )
{
	return sig_catch( SIGINT, f );
}
/**
ignore SIGPIPE
@see sig_catch
*/
bool sig_pipeign()
{
	return sig_catch( SIGPIPE, SIG_IGN );
}

/**
set handler for signal
@return true on success
*/
bool sig_catch( int s, void (*f)(int) )
{
	struct sigaction sa;
	sa.sa_handler = f;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	return ! sigaction(s,&sa,(struct sigaction *) 0) ? true : false;
}
