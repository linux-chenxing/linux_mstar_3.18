#!/bin/perl

	my $folder = $ARGV[0];
    my $full_folder = "";
    if($folder =~ /^\//){
        $full_folder = $folder;    
    }
    else{
    	$full_folder = `pwd`."/".$folder;
    }
    $full_folder =~ s/\n//;    
	my @git_list=`find $full_folder -type d -name '.git' | grep -v repo`;
	my $last_id = "";
	my $last_time = "";
	my $short_dir = "";
	if( -f "$full_folder/revision.sh" ){
		system("rm $full_folder/revision.sh");
	}
	foreach my $gl (0..$#git_list)
	{
		#print $git_list[$gl]."\n";
		my $project_dir = substr $git_list[$gl], 0, -5;
		#print "real dir: $project_dir\n";
		if($project_dir =~ /$folder\/(.+?)$/)
		{
			$short_dir = $1;
		}
		chdir($project_dir) or die "$!";
		my $commitID = `git log -n 1 --format="%H"`;
		$commitID =~ s/\n//g;
		my $cmd = "echo \"cd $short_dir\ngit checkout -f $commitID\ncd -\" >> $full_folder/revision.sh";
		#print "command: $cmd\n";
		system($cmd);
		my $commitTime = `git log -n 1 --format="%ct"`;
		if( $last_time eq "" || $last_time < $commitTime)
		{
			$last_time = $commitTime;
			$last_id = $commitID;
		}
	}
	print substr($last_id,0,7)."\n";

