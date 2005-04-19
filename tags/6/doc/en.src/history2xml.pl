#!/usr/bin/perl

open(IN, "history") or die;
@file=<IN>;
close(IN);

print "<?xml version=\"1.0\" encoding=\"iso-8859-2\"?>\n".
	"<page menu=\"history\">\n";
print "<history>";
print "<branch>";

$ver = "Branch version: ";
$ver_len = length($ver);
$date = "date: ";
$date_len = length($date);
$author = "author: ";
$author_len = length($author);

for($i=0; $i<@file.""; ++$i) {
	$file[$i] =~ s/&/&amp;/g;
	$file[$i] =~ s/</&lt;/g;
	$file[$i] =~ s/>/&gt;/g;
	
	if( $file[$i] eq "----------------------------\n" ) {
		print "</info></branch><branch>";
		next;
	}
	
	if( substr($file[$i], 0, $author_len) eq $author ) {
		print "<author>";
		print substr($file[$i], $author_len);
		print "</author><info>";
		next;
	}

	if( substr($file[$i], 0, $date_len) eq $date ) {
		print "<date>";
		print substr($file[$i], $date_len);
		print "</date>";
		next;
	}

	if( substr($file[$i], 0, $ver_len) eq $ver ) {
		print "<version>";
		print substr($file[$i], $ver_len);
		print "</version>";
		next;
	}

	print $file[$i]."<br/>";
}

print "</branch>";
print "</history>";
print "</page>\n";
