#!/usr/bin/perl

package dom_rm;
use strict;
use Test;
use conf;

BEGIN { plan tests => 2 }

foreach my $d (@conf::domains) {
	my $cmd = "dom_rm -q \"$d\"";
	`$cmd`;
	ok($?>>8, 0);
	ok($? & 127, 0);
}
