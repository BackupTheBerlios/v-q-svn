#include <signal.h>

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
