#!/usr/bin/perl

use Pg;

# Simple script that will check speed difference between using PRIMARY KEY
# which is integer and string.

$con = Pg::connectdb("user=mail dbname=mail");
if($con->status != PGRES_CONNECTION_OK) {
	die($con->errorMessage);
}

$tab = @ARGV."" ? "test1" : "test2";
if(@ARGV."") {
	$qr = $con->exec(
		"CREATE TABLE test1 (id int not null, str text not null,PRIMARY KEY(id))");
} else {
	$qr = $con->exec(
		"CREATE TABLE test2 (id int not null, str text not null,PRIMARY KEY(str))");
}
if($qr->resultStatus != PGRES_COMMAND_OK) {
	die($con->errorMessage);
}

@chars = ('q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
	'z', 'c', 'v', 'b', 'n', 'm',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	'Z', 'C', 'V', 'B', 'N', 'M');

for($i=0; $i<10000; ) {
	my $id = int(rand(0xfffffff));
	my $len = int(rand(256));
	my $val = "";
	for($j=0; $j<$len; ++$j) {
		$val .= $chars[int(rand(@chars.""-1))];
	}
	
	my $qr= $con->exec("INSERT INTO $tab VALUES($id, '$val')");
	if($qr->cmdTuples == 1) {
		++$i;
		if(@ARGV."") {
			push(@ids, $id);
		} else {
			push(@ids, $val);
		}
	} else {
		warn($con->errorMessage);
	}
}

for($i=0; $i<@ids.""; ++$i) {

	my $res = $con->exec(
		"SELECT * FROM $tab WHERE ".(@ARGV."" ? "id=$ids[$i]" : "str='$ids[0]'"));
	if($res->ntuples != 1) {
		warn($con->errorMessage);
	}
}

$con->exec("DROP TABLE $tab");
