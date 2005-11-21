#!/usr/bin/perl

package main;
$Getopt::Std::STANDARD_HELP_VERSION = 1;
sub VERSION_MESSAGE {
	my $out = shift;
	print $out "$0 version 6\n";
}

sub HELP_MESSAGE {
	my $out = shift;
	print $out "usage: $0 [-c con_info] [-C con_info] [-u user] [-U user] [-p pass] [-P pass] [-s schema] [-S schema] domains\n";
	print $out "-c con_info\tPostgreSQL connection info\n";
	print $out "-C con_info\tPostgreSQL connection info (for old database)\n";
	print $out "-u user\tconnect as this user\n";
	print $out "-U user\tconnect as this user (for old database)\n";
	print $out "-p pass\tuse this password\n";
	print $out "-P pass\tuse this password (for old database)\n";
	print $out "-s schema\tcreate objects in specified schema\n";
	print $out "-S schema\tread objects from specified schema\n";
}

package vqpgsql;

use DBI;
use Getopt::Std;

# Configuration
my %opts = ();
getopts('c:u:p:s:C:U:P:S:', \%opts);

my $con_conf = $opts{'c'} ? $opts{'c'} : "dbname=mail host=/tmp";
my $fcon_conf = $opts{'C'} ? $opts{'C'} : "dbname=mail host=/tmp";
my $con_user = $opts{'u'} ? $opts{'u'} : "mail";
my $fcon_user = $opts{'U'} ? $opts{'U'} : "mail";
my $con_pass = $opts{'p'} ? $opts{'p'} : "mail";
my $fcon_pass = $opts{'P'} ? $opts{'P'} : "mail";
my $schema = $opts{'s'} ? $opts{'s'} : "mail";
my $fschema = $opts{'S'} ? $opts{'S'} : "public";

# Prototypes
sub qdie($$);
sub version_get;

sub dom_id($);

############
my $con = DBI->connect("dbi:Pg:$con_conf", $con_user, $con_pass);
if( $con == undef ) {
	die("Can't connect");
}
if( $con->err != PGRES_CONNECTION_OK ) {
	die("Can't connect: ".$con->errstr);
}

my $fcon = DBI->connect("dbi:Pg:$fcon_conf", $fcon_user, $fcon_pass);
if( $fcon == undef ) {
	die("Can't connect");
}
if( $fcon->err != PGRES_CONNECTION_OK ) {
	die("Can't connect: ".$fcon->errstr);
}

my $ver = version_get();
print "Database version is: $ver\n";

if( $ver != 6 ) {
	die("Sorry, I know how to copy old data only to database version 6...\n");
}

foreach $dom (@ARGV) {
	my $tb = $dom;
	$tb =~ tr/.-/__/;
	print "Copying data from $dom (table $fschema.$tb)...\n";
	my $id = dom_id($dom);
	print "Domain's id is: $id\n";
	die("It should not be <=0, did you run dom_add $dom?") if($id < 0);

	# Copy users
	my $fqr = "SELECT * FROM $tb";
	my $fsth = $fcon->prepare($fqr);
	$fsth->execute;
	if( $con->err != PGRES_TUPLES_OK ) {
		qdie($fqr, $fcon);
	}
	for( $i=0, $s=$fsth->rows; $i<$s; ++$i ) {
		my @fvals = $fsth->fetchrow_array;
		my $res = `user_add ""`
	}
}

# Functions

##
#
sub qdie($$) {
	die("Query: $_[0]: ".$_[1]->errstr);
}

##
# Get v-q schema version
sub version_get {
	my $qr = "SELECT '1' FROM pg_tables WHERE schemaname='"
		.($schema eq "" ? "public" : $schema)."' AND tablename='vq_info'";
	my $sth = $con->prepare($qr);
	$sth->execute;
	my $res = $sth->fetchall_arrayref;
	if( $con->err != PGRES_TUPLES_OK ) {
		qdie($qr, $con);
	}
	if( @{$res}."" == 1 && ${$res}[0][0] eq "1" ) {
		$qr = "SELECT value FROM vq_info WHERE key='version'";
		$sth = $con->prepare($qr);
		$sth->execute;
		$res = $sth->fetchall_arrayref;
		if( $con->err != PGRES_TUPLES_OK ) {
			qdie($qr, $con);
		}
		return @{$res}."" >= 1 ? int(${$res}[0][0]) : 0;
	} else {
		return 0;
	}
}

##
#
sub dom_id($) {
	my $name = shift;
	my $qr = "SELECT DOM_ID('$name')";
	my $sth = $con->prepare($qr);
	$sth->execute;
	my $res = $sth->fetchall_arrayref;
	if( $con->err != PGRES_TUPLES_OK ) {
		qdie($qr, $con);
	}
	return @{$res}."" >= 1 ? int(${$res}[0][0]) : 0;
}

