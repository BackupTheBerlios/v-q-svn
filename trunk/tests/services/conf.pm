package conf;

use strict;

sub new {
	my ($class, %params) = (@_);
	my $self;

	$self = {
	};

	return bless $self, $class;
};

sub domains {
	return ("teest.pl", "zxcvxcv.xcv.pl");
};

sub users {
	return ("asd", "zxcvxcv", "xcv", "ersdf", "cv", "xascv", "as");
};

sub servers {
	return (
		pop3 => 'localhost',
		smtp => 'localhost',
		imap => 'localhost',
	);
};

1;
