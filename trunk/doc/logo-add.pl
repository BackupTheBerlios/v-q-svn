#!/usr/bin/perl
$berlios_gid = 5270;
$sf_gid = 87916;

foreach $arg (@ARGV) {
	open(IN, $arg) or die($arg);
	@text = <IN>;
	close(IN) or die($arg);
	for( $i=@text."", $c=0; $i!=0 && $c<15; --$i, ++$c ) {
		if( $text[$i] =~ /><\/BODY/ ) {
			$text[$i] =~ s#></BODY#><DIV id="hosted">Hosted by: <a href="http://developer.berlios.de"><img src="http://developer.berlios.de/bslogo.php?group_id=$berlios_gid" width="124" height="32" border="0" alt="BerliOS Logo" /></a> <a href="http://www.sourceforge.net"><img src="http://www.sourceforge.net/sflogo.php?group_id=$sf_gid&type=1" border="0" alt="SourceForge.net Logo" /></a></DIV>\n</BODY#;
			last;
		}
	}
	open(OUT, ">$arg.tmp") or die($arg);
	print OUT @text;
	if( ! close(OUT) ) { unlink("$arg.tmp"); die($arg); }
	rename("$arg.tmp", "$arg") or die($arg);
}
