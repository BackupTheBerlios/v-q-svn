#!/usr/bin/perl

package dom_add;
use strict;
use Test;
use conf;

BEGIN { plan tests => 2 }

my $cmd = "dom_add -q ".join(" ", @conf::domains);
`$cmd`;
ok($?>>8, 0);
ok($? & 127, 0);
