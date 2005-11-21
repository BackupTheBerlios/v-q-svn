#!/usr/bin/perl

package dom_add;
use strict;
use Test;
use conf;

BEGIN { plan tests => 0 }

my $conf = new conf;

foreach my $d ( $conf->domains ) {
	my $cmd = "dom_add -q \"$d\"";
	print "$cmd\n";
	`$cmd`;
	ok($?>>8, 0);
	ok($? & 127, 0);
}
