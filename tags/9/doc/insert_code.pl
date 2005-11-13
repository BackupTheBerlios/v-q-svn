#!/usr/bin/perl
package insert_code;

use Cwd;

if(@ARGV."" < 2) {
	die("usage: $0 directory extension [extension ...]\n".
		"Inserts code to files with names given in arguments.\n".
		"Example: $0 . .xml\n".
		"Searches in current directory recursively, adds code to .xml files.\n");
}

my $dir = $ARGV[0];
splice(@ARGV, 0, 1);

&rec($dir, $dir, \@ARGV);

#
sub rec {
	my $full = shift;
	my $dir = shift;
	my $exts = shift;
	my $cur = getcwd;

	chdir($dir) or die("chdir $dir $!");
	opendir(DIR, ".") or die("can't read $full $!");
	my @files = grep {!/^\./} readdir(DIR);
	closedir(DIR);
	foreach $f (@files) {
		if( -d $f ) {
			&rec("$full/$f", $f, $exts);
			next;
		}
		foreach $e ( @{$exts} ) {
			if( substr($f, -length($e)) eq $e ) {
				print "$full/$f: ";
				if( ! open(IN, $f) ) {
					warn("open $full/$f $!");
					last;
				}
				my @cnt = <IN>;
				close(IN);

				my $mod = 0; # file was modified?
				my $tag = "<!--INSERT:";
				my $tag_pl_beg = "<programlisting><![CDATA[";
				my $tag_pl_end = "]]></programlisting>";
				my $res = "OK";

				for ($i=0; $i<@cnt.""; ++$i) {
					if( substr($cnt[$i], 0, length($tag)) eq $tag 
						&& $cnt[$i] =~ /-->/ ) {

						my $ins = substr($cnt[$i], length($tag),
							index($cnt[$i], "-->")-length($tag));

						if( ! open(ADD,"$ins") ) {
							$mod = 0;
							$res = "open $ins $!";
							last;
						}
						my $cnt_add = join("", <ADD>);
						if( ! close(ADD) ) {
							$mod = 0;
							$res = "close $ins $!";
							last;
						}

						if( $i+1 != @cnt."" 
							&& substr($cnt[$i+1], 0, 
								length($tag_pl_beg)) eq $tag_pl_beg ) {

							while( @cnt."" != $i+1 ) {
								my $l = $cnt[$i+1];
								splice(@cnt, $i+1, 1);
								last if( substr($l, 0, length($tag_pl_end)) 
									eq $tag_pl_end );
							}
							if( @cnt."" == $i+1 ) {
								$res = "can't find $tag_pl_end";
								$mod = 0;
								last;
							}
						}
						$cnt[$i] .= $tag_pl_beg."\n".$cnt_add."\n".$tag_pl_end."\n";
						$mod = 1;
					}
				}
				if( $res eq "OK" && $mod ) {
					if( open(OUT, ">$f.tmp") ) {
						for( $i=0; $i<@cnt.""; ++$i ) {
							print OUT $cnt[$i];
						}
						if( ! close(OUT) ) {
							warn("close $f.tmp $!");
						} elsif( ! rename("$f.tmp", $f) ){
							warn("rename $f.tmp->$f $!");
						} else {
							print "$res\n";
						}
					} else {
						warn("open $f.tmp $!");
					}
				} else {
					print "$res\n";
				}
				last;
			}
		}
	}
	chdir($cur) or die("chdir $cur $!");
}
