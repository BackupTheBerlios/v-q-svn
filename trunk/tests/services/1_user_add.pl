#!/usr/bin/perl

package user_add;

use strict;
use Test;
use conf;

BEGIN { plan tests => 0 }

my $conf = new conf;

foreach my $d ($conf->domains) {
	foreach my $u ($conf->users) {
		my $cmd = "user_add \"$u\@$d\" \"$u\"";
		`$cmd`;
		ok($?>>8, 0);
		ok($? & 127, 0);
	}
}

