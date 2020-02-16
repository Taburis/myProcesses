
import subprocess
import sys


inlist = []
INPUT
output = OUTPUT


for f in inlist:
	if not os.path.exists(f): 
		print 'Abort: waiting for all jobs done.'
		sys.exit()

print 'jobs done, starting the last merge step...'
cmd = ['hadd','-f', out]+inlist
subprocess.call(cmd)
