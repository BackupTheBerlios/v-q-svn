#!/usr/bin/perl

package dom_add;

use strict;
use Test;

BEGIN { plan tests => 4 }

my @domains = ( "test.pl", "asd.pl", "ZXCZXC.plasd.asd" );
my $bin = "$ENV{'BASE'}/interfaces/clue";

my $cmd = "$bin/dom_add ".join(" ", @domains);
`$cmd`;
ok($?>>8, 0);
ok($? & 127, 0);

$cmd = "$bin/dom_rm ".join(" ", @domains);
`$cmd`;
ok($?>>8, 0);
ok($? & 127, 0);
