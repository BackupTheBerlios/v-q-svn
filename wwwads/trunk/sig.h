#ifndef SIG_H
#define SIG_H

bool sig_chld_nocldwait();
bool sig_pipe( void (*)(int) );
bool sig_catch( int, void (*)(int) ); 
bool sig_pipeign();
bool sig_term( void (*)(int) );
bool sig_int( void (*)(int) );

#endif
