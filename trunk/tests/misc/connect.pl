#!/usr/bin/perl

package main;
$Getopt::Std::STANDARD_HELP_VERSION = 1;
sub VERSION_MESSAGE {
	my $out = shift;
	print $out "$0\n";
}

sub HELP_MESSAGE {
	my $out = shift;
	print $out "usage: $0 [-c #connections|-t #seconds]\n";
	print $out "-t #\tnumber of seconds script will run\n";
	print $out "-c #\tnumber of connections (default: 1000)\n";
	print $out "-f\tadd random errors\n";
	print $out "-l hostname\tmachine to connect using SMTP and POP3 (default: localhost)\n";
	print $out "<stdin> should be a list of e-mail addresses that will be used\n";
	print $out "\t(password should be the same as e-mail address)\n";
}

package connect;

use Getopt::Std;
use Net::SMTP;
use Net::POP3;

# Configuration

my %opts = ();
getopts('t:c:l:f', \%opts);

my $secs = $opts{'t'} ? $opts{'t'} : 0;
my $cons_cnt = $opts{'c'} ? $opts{'c'} : 1000;
my $host = $opts{'l'} ? $opts{'l'} : "localhost";
my $errors = $opts{'f'} ? $opts{'f'} : 0;

my @emails = ();
while(<>) {
	chomp if( /\n/ );
	push(@emails, $_);
}

if( ! @emails."" ) {
	print STDERR "No e-mails specified, quiting...\n";
	exit 1;
}

my $loop = 0;
if( $secs > 0 ) {
	$loop = 1;
	$SIG{ALRM} = sub {exit;};
	alarm($secs);
}

for( my $i=0; $i < $cons_cnt || $loop; ++$i ) {
	my $idx = int(rand(@emails.""));
	my $type = int(rand(2));
	print $emails[$idx].": ";

	my $pass = $emails[$idx];
	my $login = $emails[$idx];

	if( $errors && rand(10) <= 3 ) {
		$te = int(rand(10));
		if( $te < 3 ) {
			$login = $login.".".$te;
		} elsif( $te < 6 ) {
			$login = $te.".".$login;
		} else {
			$pass = $te;
		}
	}
	
	if( $type ) {
		print "SMTP: ";
		my $smtp = Net::SMTP->new($host);
		if( $smtp->auth( $login, $pass )
			&& $smtp->mail("#\@<>")
			&& $smtp->to($emails[$idx])
			&& $smtp->data()
			&& $smtp->datasend("Subject: $0\n\n$emails[$idx]\n")
			&& $smtp->dataend()
			&& $smtp->quit() ) {
			print "OK";
		} else {
			print "ERROR";
		}
	} else {
		print "POP3: ";
		my $pop = Net::POP3->new($host);
		if( $pop->login($login, $pass)
			&& $pop->quit() ) {
			print "OK";
		} else {
			print "ERROR";
		}
	}
	print "\n";
}

1;
