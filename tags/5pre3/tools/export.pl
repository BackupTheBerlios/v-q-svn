#!/usr/bin/perl

package export;

use Cwd;

if( @ARGV."" != 1 ) {
	die("please give me version name (i.e. 5pre3)");
}

my $dir = &getcwd;

if( substr($dir, -6) ne "/trunk" ) {
	die("please run me from trunk directory");
}

if( -e "../$ARGV[0]" ) {
	die("../$ARGV[0] exists");
}

system "svn", "export", ".", "../$ARGV[0]";
if( ! (($? >> 8) == 0 && ($? & 127) == 0) ) {
	die("svn execution failed");
}

chdir ".." or die("chdir .. failed");
system "tar cf - $ARGV[0] | bzip2 -c9 > $ARGV[0].tar.bz2";
if( ! (($? >> 8) == 0 && ($? & 127) == 0) ) {
	unlink("$ARGV[0].tar.bz2");
	die("compression failed");
}

