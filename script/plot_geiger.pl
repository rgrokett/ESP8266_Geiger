#!/usr/bin/perl
# plot_geiger.pl
#
# sudo apt-get install libchart-perl
# 01/2016

use Chart::Points;
#use Chart::Lines;
use Net::FTP;

$DEBUG = 0;
$TEST = 0;

my $chart = new Chart::Points(1200, 300);
#my $chart = new Chart::Lines(300, 300);

# FIND CURRENT TIME IN UNIX SECONDS - (3600 * 4) four hours
$curtime = time;
$maxhrs = ($curtime - (3600 *6 ));
if ($TEST) { $maxhrs = ($curtime - (3600 *9999 )); }

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);

 my @abbr = qw( Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec );
#$year = sprintf("%02d", $year % 100);
$year += 1900;

$time_str = "$abbr[$mon] $mday,$year";

my %property;
$property{'title' } = "Geiger Counter - $time_str";
$property{'x_label'} = 'Time (10 Min)';
$property{'y_label'} = 'uSv/h';
$property{'legend'} = 'none';
$property{'min_val'} = 0.0;
#$property{'max_val'} = 5;
$property{'x_grid_lines'} = 'false';
$property{'y_grid_lines'} = 'true';
$property{'include_zero'} = 'true';
$property{'integer_ticks_only'} = 'false';
$property{'skip_int_ticks'} = 10;
$property{'pt_size'} = 10;
$property{'sort'} = 'false';
$property{'xy_plot'} = 'false';
$property{'precision'} = '2';

$chart->set(%property);


# OPEN LIGHTNING DATA FILE /www/docs/lightning.txt
$DIR = "/www/docs/geiger";
$out = `tail -100 $DIR/geiger.log > $DIR/geiger.dat`;
$DATAFILE = "$DIR/geiger.dat";
if ($TEST) { $DATAFILE = "/www/docs/geiger/geigerTEST.log"; }

open (MYFILE, $DATAFILE);

# FORMAT DATE YYYY-MM-DDESTHH:MM:SS|CountsPerMinute|uSieverts/hour
#dt=2016-01-08EST19:27:03|cpm=[23]|usvh=[0.19]


$reccnt = 0;

# LOOP FOR EACH ENTRY
 while ($line = <MYFILE>) {
        # GET RID OF CR LF
        $line =~ s/\r//g,$line;
        chomp ($line);

        if ($DEBUG) { print "LINE: [$line]\n"; }

        # SPLIT INTO TIME AND DISTANCE
        ($logtime, $cpm, $usvh) = split /\|/,$line;

	# CURRENTLY PLOT ALL DATA
          # SPLIT MESSAGE
          my ($x, $dt) = split/\=/,$logtime;
          my ($x, $cpm) = split/\=/,$cpm;
          my ($x, $usvh) = split/\=/,$usvh;

          if ($DEBUG) { print "LOGTIME: [$logtime] MSG: [$cpm][$usvh]\n"; }

          # ADD X-AXIS TIME TO ARRAY
          push (@time_array, "x");

          # ADD Y-AXIS DISTANCE
          push (@cpm_array, "$cpm");
          push (@usvh_array, "$usvh");

          # COUNT
          $reccnt++;
 }

close (MYFILE);

# TEST
# ADD X-AXIS TIME TO ARRAY
#@time_array = (20,18,15,10,9,8,8,6,4,2,2,2);

# ADD Y-AXIS DISTANCE
# ADD Y-AXIS DISTANCE
#@dist_array = (22,18,20,15,5,13,6,14,12,26,43,35);

# ADD STRIKE COUNT
$chart->set ('sub_title' => "Total recs: ".$reccnt );
# ADD TO PLOT
# X-AXIS TIME
$chart->add_dataset(@time_array);
# Y-AXIS DISTANCE
#$chart->add_dataset(@cpm_array);
$chart->add_dataset(@usvh_array);

$chart->png('/www/docs/geiger/geiger.png');


# FTP TO WEBSITE
my $host = "www.grokett.org";
my $user = "grokett2";
my $password = "oehkBdTU";

#my $f = Net::FTP->new($host) or die "Can't open $host\n";
#$f->login($user, $password) or die "Can't log $user in\n";

#my $dir = "public_html";
#$f->cwd($dir) or die "Can't cwd to $dir\n";

#$f->binary();
#my $file_to_put = "/www/docs/lightning.png";
#$f->put($file_to_put) or die "Can't put $file into $dir\n";

#$f->quit();


