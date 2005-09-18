#!/usr/bin/perl

package pop3;

use strict;
use Test;
use conf;
use Mail::POP3Client;

my $conf = new conf;

plan( tests => 0 );

foreach my $d ($conf->domains) {
		foreach my $u ($conf->users) {
				&do_pop3($d, $u);	
		}
}               

sub do_pop3 {
	my $domain = shift;
	my $login = shift;

	my %sers = $conf->servers;

	my $pop = new Mail::POP3Client( HOST => $sers{pop3}, 
		USER => "$login\@$domain",
		PASSWORD => "$login",
		AUTH_MODE => 'APOP' );
	ok(!index($pop->Message,"+OK"));
	$pop->Close();

	my $pop = new Mail::POP3Client( HOST => $sers{pop3}, 
		USER => "$login\@$domain",
		PASSWORD => "$login",
		AUTH_MODE => 'PASS' );
	ok(!index($pop->Message,"+OK"));
	$pop->Close();

	my $pop = new Mail::POP3Client( HOST => $sers{pop3}, 
		USER => "$login\@$domain",
		PASSWORD => "123$login",
		AUTH_MODE => 'PASS' );
	ok(!index($pop->Message,"PASS failed:"));
	$pop->Close();

	my $pop = new Mail::POP3Client( HOST => $sers{pop3}, 
		USER => "$login\@$domain",
		PASSWORD => "123$login",
		AUTH_MODE => 'APOP' );
	ok(!index($pop->Message,"APOP failed:"));
	$pop->Close();
}
