#!/usr/bin/perl

use File::Temp qw/ tempfile /;

use Cwd;
$ARGV[0] = cwd;
$ARGV[0] = substr( $ARGV[0], rindex($ARGV[0], '/')+1);

($fd,$name) = tempfile("/tmp/webalizer_wrap.XXXXXX");
while(<STDIN>) {
    print $_;
    print $fd $_;
}

# don't wait for children
$SIG{CHLD} = "IGNORE";

# run webalizer_log.pl
if( ! fork() ) {
    system "webalizer_log.pl", $ARGV[0], $name;
    unlink($name);
    exit(0);
}
exit 0;
