/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

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

#include "sig.hpp"

#include <csignal>
#include <cstdlib>

namespace sys {
		
	using namespace std;
	
	/**
	 * set handler for SIGPIPE 
	 * \return true on success
	 */
	bool sig_pipe( void (*f)(int) ) {
		return sig_catch( SIGPIPE, f );
	}
	
	/**
	 *
	 */
	bool sig_term( void (*f)(int) ) {
		return sig_catch( SIGTERM, f );
	}
	
	/**
	 * Set handler for SIGINT
	 */
	bool sig_int( void (*f)(int) ) {
		return sig_catch( SIGINT, f );
	}
	/**
	 * ignore SIGPIPE
	 * \see sig_catch
	 */
	bool sig_pipe_ign() {
		return sig_catch( SIGPIPE, SIG_IGN );
	}
	
	/**
	 * set handler for signal
	 * \return true on success
	 */
	bool sig_catch( int s, void (*f)(int) ) {
		struct sigaction sa;
		sa.sa_handler = f;
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		return ! sigaction(s,&sa,(struct sigaction *) 0) ? true : false;
	}
	
	/** 
	 * Set handler for SIGCHLD
	 * \see sig_catch
	 */
	bool sig_child( void (*f) (int) ) {
		return sig_catch(SIGCHLD, f);
	}
	
	/**
	 * Set default handler for signal
	 * \param sig signal
	 * \see sig_catch
	 */
	bool sig_default( int sig ) {
		return sig_catch(sig, SIG_DFL);
	}
	
	/**
	 * Pause and wait on any signals.
	 * \return true if process was stopped
	 */
	void sig_pause() {
		sigset_t ss;
		sigemptyset(&ss);
		sigsuspend(&ss);
	}
	
	/**
	 * Block specified signal
	 * \param sig signal
	 * \return true on success
	 */
	bool sig_block( int sig ) {
		sigset_t ss;
		sigemptyset(&ss);
		sigaddset(&ss,sig);
		return !sigprocmask(SIG_BLOCK,&ss,(sigset_t *) 0);
	}
	
	/**
	 * Unblock signal
	 * \param sig signal
	 * \return true on success
	 */
	bool sig_unblock( int sig ) {
		sigset_t ss;
		sigemptyset(&ss);
		sigaddset(&ss,sig);
		return !sigprocmask(SIG_UNBLOCK,&ss,(sigset_t *) 0);
	}

} // namespace sys
