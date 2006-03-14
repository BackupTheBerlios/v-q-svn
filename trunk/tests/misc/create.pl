#!/usr/bin/perl

package main;
$Getopt::Std::STANDARD_HELP_VERSION = 1;
sub VERSION_MESSAGE {
	my $out = shift;
	print $out "$0\n";
}

sub HELP_MESSAGE {
	my $out = shift;
	print $out "usage: $0 [-d #domains] [-u #users]\n";
	print $out "-d #\tnumber of domains to create\n";
	print $out "-u #\tnumber of users to create\n";
}

package random;

use Getopt::Std;

# Configuration

my %opts = ();
getopts('d:u:', \%opts);

my $users_cnt = $opts{'u'} ? $opts{'u'} : 100;
my $doms_cnt = $opts{'d'} ? $opts{'d'} : 10;

my $suffix =  ".random.pl";

@doms = ();
for( my $i=0; $i<$doms_cnt; ++$i ) {
	my $dom = "$i$suffix";
	$str=`dom_add "$dom"`;
	if( $? >> 8 == 0 || $str =~ /#18:/ ) {
		for( my $j=0; $j<$users_cnt; ++$j ) {
			my $user = "$j\@$dom";
			$str=`user_add "$user" "$user"`;
			if( $? >> 8 == 0 || $str =~ /#18:/ ) {
				print "$user\n";
			} else {
				print STDERR $str;
			}
		}
	} else {
		print STDERR $str;
	}
}

1;
