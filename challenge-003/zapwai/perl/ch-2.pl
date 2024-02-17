use v5.30.0;
my $N = $ARGV[0] || 3;
my @L;
$L[0][0] = 1;
$L[1][0] = 1;
$L[1][1] = 1;

for my $i (2 .. $N) {		# rows
    for my $j (0 .. $i) {
	if ($j < 1) {
	    $L[$i][$j] = 1;
	} elsif ($j > $i - 1) {
	    $L[$i][$j] = 1;	    
	} else{
	    $L[$i][$j] = $L[$i - 1][$j - 1] + $L[$i - 1][$j];
	}
#	$L[$i][$j] = ($j < 1) ? { 1 } : { $L[$i - 1][$j - 1] + $L[$i - 1][$j] }
	    
    }
}
for (0 .. $N) {
    print " " x ($N - $_);
    say "@{$L[$_]}"
}
