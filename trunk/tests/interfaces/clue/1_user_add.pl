#!/usr/bin/perl

package dom_ad;

use strict;
use Test;

BEGIN { plan tests => 8 }

print "# Testing dom_add, user_add, user_pass, dom_rm\n";

my @domains = ( "test.pl", "asd.pl", "ZXCZXC.plasd.asd" );
my @users = ( "asda", 'asdasd', 'asd324' );
my $bin = "$ENV{'BASE'}/interfaces/clue";

my $cmd = "$bin/dom_add -q ".join(" ", @domains);
`$cmd`;
ok($?>>8, 0);
ok($? & 127, 0);

my $emails = "";
my $emails1 = "";
foreach my $d (@domains) {
	foreach my $u (@users) {
		$emails .= "$u\@$d PASS ";
		$emails1 .= "$u\@$d PASS1 ";
	}
}

$cmd = "$bin/user_add -q $emails";
print `$cmd`;
ok($?>>8, 0);
ok($? & 127, 0);

$cmd = "$bin/user_pass -q $emails1";
print `$cmd`;
ok($?>>8, 0);
ok($? & 127, 0);

$cmd = "$bin/dom_rm -q ".join(" ", @domains);
`$cmd`;
ok($?>>8, 0);
ok($? & 127, 0);
