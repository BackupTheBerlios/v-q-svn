#!/usr/bin/perl

package dom_ad;

use strict;
use Test;

BEGIN { plan tests => 2 }

my @domains = ( "test.pl", "asd.pl", "ZXCZXC.plasd.asd" );
my $bin = "$ENV{'BASE'}/interfaces/clue";

my $cmd = "$bin/dom_add ".join(" ", @domains);
`$cmd`;
ok($? == 0);

$cmd = "$bin/dom_rm ".join(" ", @domains);
`$cmd`;
ok($? == 0);
