#!/usr/bin/perl

package user_add;

use strict;
use Test;
use conf;

BEGIN { plan tests => int(@conf::domains."") * int(@conf::users."") * 2 }

foreach my $d (@conf::domains) {
	foreach my $u (@conf::users) {
		my $cmd = "user_add -q \"$u\@$d\" \"$u\"";
		`$cmd`;
		ok($?>>8, 0);
		ok($? & 127, 0);
	}
}

