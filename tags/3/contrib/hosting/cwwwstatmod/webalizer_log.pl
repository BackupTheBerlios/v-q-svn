#!/usr/bin/perl

use Pg;
use File::Temp qw/ tempfile tempdir /;

if( @ARGV."" != 2 ) {
    print STDERR "usage: $0 domain filename\n";
    exit 111;
}

$con = Pg::connectdb("dbname=poczta user=dns_select password=^*df234ASD466");
if( $con->status != PGRES_CONNECTION_OK ) {
    print STDERR "$0: can't connect: ".$con->errorMessage."\n";
    exit 111;
}

# domain's aliases
$dall = $con->exec("select da.name from domains_all as da,domains as d ".
        "where d.domain='$ARGV[0]' and da.domain_id=d.id".
        " AND da.name!='$ARGV[0]'");

if( $dall->resultStatus != PGRES_TUPLES_OK ) {
    die("$0: can't get list of names: ".$con->errorMessage."\n");
}
        
# prefixes
$pre = $con->exec("select dp.prefix || '.' || da.name from ".
        "domains_prefixes as dp,domains_all as da,".
        "domains as d where d.domain='$ARGV[0]' and da.domain_id=d.id".
        " and dp.domain_id=da.id");

if( $pre->resultStatus != PGRES_TUPLES_OK ) {
    die("$0: can't get list of names: ".$con->errorMessage."\n");
}

for( $i=0, $ntup=$dall->ntuples; $i<$ntup; ++$i ) {
    my $domain = $dall->getvalue($i, 0);
    push(@names,$domain);
}
$dall = undef;
for( $i=0, $ntup=$pre->ntuples; $i<$ntup; ++$i ) {
    my $domain = $pre->getvalue($i, 0);
    push(@names,$domain);
}
$pre = undef;
$con = undef;

# create temporary directory, auto-cleaned on exit
$dir = tempdir( CLEANUP => 1 );

# create temp. file for main domain
($fmain, $fnmain) = tempfile( DIR => $dir, UNLINK => 0 );

# for each sub-domain create a temporary file
foreach $name (@names) {
    my ($fh, $fname) = tempfile( DIR => $dir, UNLINK => 0 );
    push(@files,$fh);
    push(@fnames,$fname);
}

open(IN, $ARGV[1]) or die("open: $ARGV[1]");

# read in, demultiplex
while(<IN>) {
    my $space = index($_, " ");
    my $dom = substr( $_, 0, $space );
    my $log = substr( $_, $space+1 );
    # forward to stdout (to multilog's processed)
    #print $_;

    # looking for domain
    for( $i=0; $i < @names.""; ++$i ) {
        if( $dom eq $names[$i] ) {
            my $fh = $files[$i];
            print $fh $log;
            last;
        }
    }
    if( $i == @names."" ) {
        print $fmain $log;
    }
}
close(IN);

$EEXIST = 17;

# run webalizer for main domain
if(! mkdir("/var/www/htdocs/hosting/log/$ARGV[0]") && $! != $EEXIST ) {
    die($!);
}
if(! mkdir("/var/www/htdocs/hosting/log/$ARGV[0]/$ARGV[0]") && $! != $EEXIST ) {
    die($!); 
}
close($fmain);
system "webalizer", "-Q", "-n", $ARGV[0], "-o", 
    "/var/www/htdocs/hosting/log/$ARGV[0]/$ARGV[0]",
    $fnmain;
    
# for each sub-domain run webalizer
for( $i = 0; $i < @fnames.""; ++$i ) {
    if(! mkdir("/var/www/htdocs/hosting/log/$ARGV[0]/$names[$i]") && $! != $EEXIST ) {
        die($!);
    }
    close($files[$i]); # close fd
    system "webalizer", "-Q", "-n", $names[$i], "-o", 
        "/var/www/htdocs/hosting/log/$ARGV[0]/$names[$i]",
        $fnames[$i];
}

system "find", "/var/www/htdocs/hosting/log/$ARGV[0]", "-type", "f",
    "-exec", "chmod", "644", "{}", ";", "-o", "-type", "d", "-exec", "chmod",
    "755", "{}", ";";
