#!/usr/bin/perl
use 5.010;

open OUT, ">$ARGV[0]" or die $!;
$ARGV[0] =~ m/(\d)/;
$idnum = $1;
open IN,"sta_$idnum.txt" or die $!;

$cnt=0;
while(<IN>){
	($strleft) = split ;
	$line = <IN>;
	($strright) = split /\s/,$line;
	say OUT ">$cnt\n$strleft\t$strright"; 
	$cnt++;
}
