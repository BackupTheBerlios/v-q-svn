#!/usr/bin/perl
@projects = (
	"../../../../vq",
	"../../../contrib/freemail",
	"../../../contrib/host_add",
	"../../../contrib/hosting",
	"../../../contrib/pg2fs",
	"../../../contrib/valid",
	"../../../contrib/vqsignup",
	"../../../contrib/vqsignup1",
	"../../../contrib/vquserrm",
	"../../../contrib/vqwww"
);

print '<?xml version="1.0"?><page menu="files"><conf>';
foreach $proj (@projects) {
	my $idx = rindex($proj, '/');
	my $name = $idx ? substr($proj, $idx+1) : $proj;
	next if ! -d $proj;
	
	opendir(DIR, "$proj/conf") or die;
	my @files = readdir(DIR);
	closedir(DIR);
	
	print '<proj name="'.$name.'">';
	foreach $file (@files) {
		my $path = "$proj/conf/$file";
		next if ! -f $path;
		open(IN, $path) or die;
		print '<file name="'.$file.'">';
		my @text = <IN>;
		close(IN);
		splice(@text, 0, 1);
		print "@text";
		print '</file>';
	}
	print '</proj>';
}
print "</conf></page>\n";
