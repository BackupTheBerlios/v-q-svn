#!/usr/bin/perl
# skrypt przerabia pliki dat z milionerow pod windows na pliki textowe,
# pozniej pliki textowe moga zostac zamienione na format wlasciwy
# temu programowi
#
# Pawel Niewiadomski <new@linuxpl.org>, (C) 2002, licencja GPL
#
# wywolanie:
# plik_dat plik_txt

open( I, $ARGV[0] ) or die;
open( O, ">$ARGV[1]" ) or die;

while( ! eof( I ) ) {
    read( I, $f, 280 );
    @t = unpack( "C8 C/a", $f );
    print O $t[4]."|";
    print O $t[8]."|";
   
    @t = unpack( "C159 C/Z", $f );
    print O $t[159]."|";
    
    @t = unpack( "C159 C26 C/Z", $f );
    print O $t[159+26]."|";
   
    @t = unpack( "C159 C26 C26 C/Z", $f );
    print O $t[159+26*2]."|";
   
    @t = unpack( "C159 C26 C26 C26 C/Z", $f );
    print O $t[159+26*3]."|";

    @t = unpack( "C159 C26 C26 C26 C26 C C", $f );
    print O $t[159+26*4+1]."\n";
}
close( O );
close( I );
