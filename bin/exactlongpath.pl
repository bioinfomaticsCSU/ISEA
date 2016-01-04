#!/usr/bin/perl
use 5.010;

while(<>){
	chomp;
	next if />/;
	next if (length $_ ) < 200;
	push @arr,$_;
}

@arr = sort {length $b <=> length $a} @arr;

$sum = 0;
$cnt = 0;
foreach(@arr){
	last if $sum > 500000;
	$sum += length $_;
	print ">$cnt\n$_\n";
	$cnt++;
} 