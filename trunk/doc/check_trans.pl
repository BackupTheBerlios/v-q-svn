#!/usr/bin/perl
# This scripts checks if translations are up-to-date. It does that
# by checking for all files in en.src corresponding files in
# translations directory, it compares last change revision. If translated 
# file has it lower it means it's older than original.

opendir(CUR, ".") or die;
my @trans = grep {/\.src$/ and !/^en\.src$/} readdir(CUR);
closedir(CUR);

die("no translation found") if(! @trans."");

open(LS,"svn ls -Rv en.src|") or die;
my @lns = <LS> or die;
close(LS) or die;

my %files = &lns2hash(@lns);

foreach $tran (@trans) {
	print ">>> $tran\n";
	open(LS,"svn ls -Rv \"$tran\"|") or die;
	my @lns = <LS> or die;
	close(LS) or die;

	my %tfiles = &lns2hash(@lns);
	foreach $file (keys(%files)) {
		if( exists($tfiles[$file]) 
			&& $tfiles{$file} >= $files{$file} ) {
			delete $tfiles{$file};
			next;
		}
		print "Old: $file\n";
		delete $tfiles{$file};
	}
	foreach $file (keys(%tfiles)) {
		print "Missing original: $file\n";
	}
}

sub lns2hash {
	my @lns = @_;
	my %hash = ();
	foreach my $ln (@lns) {
		chomp($ln) if($ln =~ /\n/);
		my @fs = split(" ", $ln);
		$hash{$fs[@fs.""-1]} = int($fs[0]);
	}
	return %hash;
}
