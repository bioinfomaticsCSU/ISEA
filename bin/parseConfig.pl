#!/usr/bin/perl
use Env;
use 5.010;


if(! -e "mapping"){
	`mkdir mapping`;
}

my $myhash;
$lineno = 0;
$fileid = 1;
while(<>){
	$lineno++;
	s/\s*//;
	next if $_ eq "";
	next if /^#/;
	if(/^lib_num\s*=/){
		if(exists $myhash{"lib_num"}){
			say STDERR "error in config file : lib_num is already defined.";
		}else{
			@elems = split;
			$cnt = 0;
			foreach(@elems){
				$cnt++;
				last if($cnt == 4);
				$element = $_;
				if($cnt == 1){
					if($element ne "lib_num"){
						say STDERR "error in config file, lib_num : unresolved line in ", $lineno;
						exit(-1);
					}
				}elsif($cnt == 2){
					if($element ne "="){
						say STDERR "error in config file, lib_num : unresolved line in ", $lineno;
						exit(-1);
					}
				}elsif($cnt == 3){
					if($element =~ m/[\D]/){
						say STDERR "error in config file, lib_num : unresolved line in ", $lineno;
						exit(-1);
					}
					$lib_num = $element;
				}
			}
		}
	}elsif(/^thread_num\s*=/){
		if(exists $myhash{"thread_num"}){
			say STDERR "error in config file : thread_num is already defined.";
			exit(-1);
		}else{
			@elems = split;
			$cnt = 0;
			foreach(@elems){
				$cnt++;
				last if($cnt == 4);
				$element = $_;
				if($cnt == 1){
					if($element ne "thread_num"){
						say STDERR "error in config file, thread_num : unresolved line in ", $lineno;
						exit(-1);
					}
				}elsif($cnt == 2){
					if($element ne "="){
						say STDERR "error in config file, thread_num : unresolved line in ", $lineno;
						exit(-1);
					}
				}elsif($cnt == 3){
					if($element =~ m/[\D]/){
						say STDERR "error in config file, thread_num : unresolved line in ", $lineno;
						exit(-1);
					}
					$thread_num = $element;
				}
			}
		}
	}elsif(/^k\s*=/){
		if(exists $myhash{"k"}){
			say STDERR "error in config file : k is already defined.";
			exit(-1);
		}else{
			@elems = split;
			$cnt = 0;
			foreach(@elems){
				$cnt++;
				last if($cnt == 4);
				$element = $_;
				if($cnt == 1){
					if($element ne "k"){
						say STDERR "error in config file, k  : unresolved line in ", $lineno;
						exit(-1);
					}
				}elsif($cnt == 2){
					if($element ne "="){
						say STDERR "error in config file, k  : unresolved line in ", $lineno;
						exit(-1);
					}
				}elsif($cnt == 3){
					if($element =~ m/[\D]/){
						say STDERR "error in config file, k : unresolved line in ", $lineno;
						exit(-1);
					}
					$k = $element;
				}
			}
		}
	}elsif(/^pr\s/){
		chomp;
		@elems = split;
		$cnt = 0;
		foreach(@elems){
			$cnt++;
			last if($cnt == 5);
			$element = $_;
			if($cnt == 1){
				if($element ne "pr"){
					say STDERR "error in config file : unrecognized file type $element, please use pr(paired-end) or mp(mate-pair) ";
					exit(-1);
				}
				$type = $element;
			}elsif($cnt == 2){
				if(! -e $element){
					say STDERR "file $element doestn't exist";
					exit(-1);
				}
				$left = $element;
			}elsif($cnt == 3){
				if(! -e $element){
					say STDERR "file $element doestn't exist";
					exit(-1);
				}
				open readsfile,"<$element" or die $!;
				$readsline = <readsfile>;
				$readsline = <readsfile>;
				chomp $readsline;
				$readlen = length $readsline;
				close readsfile;
				$right = $element;
			}elsif($cnt == 4){
				if($element =~ m/\-+[\D]/){
					say STDERR "error in config file : unresolved line in ", $lineno;
					exit(-1);
				}
				$ovlp = $element;
				push @prfilelines, "\t0\t0\t0\t$readlen\t$ovlp";
				push @prfilelines2, "pr\t$left\t$right\t$readlen\t$ovlp";
				$fileid++;
			}
		}
		if(exists $myhash{"$left\t$right"}){
			say STDERR "error in config file : $left\t$right is already defined.";
			exit(-1);
		}else{
			$myhash{"$left\t$right"} = "$type\t$ovlp";
		}
	}elsif(/^mp\s*/){
		chomp;
		@elems = split;
		$cnt = 0;
		foreach(@elems){
			$cnt++;
			last if($cnt == 5);
			$element = $_;
			if($cnt == 1){
				if($element ne "mp"){
					say STDERR "error in config file : unrecognized file type $element, please use pr(paired-end) or mp(mate-pair) ";
					exit(-1);
				}
				$type = $element;
			}elsif($cnt == 2){
				if(! -e $element){
					say STDERR "file $element doestn't exist";
					exit(-1);
				}
				$left = $element;
			}elsif($cnt == 3){
				if(! -e $element){
					say STDERR "file $element doestn't exist";
					exit(-1);
				}
				open readsfile,"<$element" or die $!;
				$readsline = <readsfile>;
				$readsline = <readsfile>;
				chomp $readsline;
				$readlen = length $readsline;
				close readsfile;
				$right = $element;
			}elsif($cnt == 4){
				if($element =~ m/\-+[\D]/){
					say STDERR "error in config file : unresolved line in ", $lineno;
					exit(-1);
				}
				$ovlp = $element;
				push @mpfilelines, "\t0\t0\t0\t$readlen\t$ovlp";
				push @mpfilelines2, "mp\t$left\t$right\t$readlen\t$ovlp";
				$fileid++;
			}
		}
		if(exists $myhash{"$left\t$right"}){
			say STDERR "error in config file : $left\t$right is already defined.";
			exit(-1);
		}else{
			$myhash{"$left\t$right"} = "$type\t$ovlp";
		}
	}else{
		say STDERR "error in config file : unresolved line in ", $lineno;
		say STDERR ;
		exit(-1);
	}
}

open mappinginfo,">mapping/inputfiles.info" or die $!;
open libinfo,">lib.pre.info" or die $!;

say libinfo "$lib_num\t$k";
$cnt = 1;
foreach(@prfilelines){
	say libinfo "$cnt$_";
};
for(@mpfilelines){
	say libinfo "$cnt$_";
};

say mappinginfo "$thread_num\t$k\t$lib_num";
say mappinginfo for @prfilelines2;
say mappinginfo for @mpfilelines2;
