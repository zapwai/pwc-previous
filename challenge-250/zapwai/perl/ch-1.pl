use v5.30;
my @ints = (0, 1, 2);
proc(@ints);
@ints = (4,3,2,1);
proc(@ints);
@ints = (1, 2, 3, 4, 5, 6, 7, 8, 9, 0);
proc(@ints);
sub proc {
    my @ints = @_;
    my $index = -1;
    for my $i (0 .. $#ints) {
	if ($i % 10 == $ints[$i]) {
	    $index = $i ;
	    last;
	}
    }
    say "Input: \@ints = (@ints)";
    say "Output: $index";
}
