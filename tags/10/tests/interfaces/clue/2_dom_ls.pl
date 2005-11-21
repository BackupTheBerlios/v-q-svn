#!/usr/bin/perl

package dom_ls;

use strict;
use Test;

BEGIN { plan tests => 2 }

my $bin = "$ENV{'BASE'}/interfaces/clue";

my $cmd = "$bin/dom_ls";
`$cmd`;
ok($?>>8, 0);
ok($? & 127, 0);
