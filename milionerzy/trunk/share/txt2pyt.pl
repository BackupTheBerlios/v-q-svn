#!/usr/bin/perl
# przetwarza pytania z pliku textowego do postaci akceptowalnej przez
# program milionerzy
#
# Pawel Niewiadomski <new@linuxpl.org>, (C) 2002, licencja GPL
#
# wywolanie:
# plik katalog_docelowy

open( I, $ARGV[0] ) or die( "open: $ARGV[0]: $!" );
chdir( $ARGV[1] ) or die( "chdir: $ARGV[1]: $!" );

while( <I> ) {
    @pyt = split /\|/;
    next if( @pyt."" != 7 );
    if( open( O, ">$pyt[0]" ) ) {
        print O $pyt[6],
                $pyt[1]."\n",
                $pyt[2]."\n",
                $pyt[3]."\n",
                $pyt[4]."\n",
                $pyt[5]."\n";
        close( O );
        die( "chmod: $pyt[0]: $!" ) if( ! chmod( 0644, $pyt[0] ) );
    } else {
        die( "open: $pyt[0]: $!" );
    }
    
}

close( I );
