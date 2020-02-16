
import subprocess
import sys


inlist = []
INPUT
output = OUTPUT


for f in listf:
	if not os.path.exists(f): 
		print 'Abort: waiting for all jobs done.'
		sys.exit()

print 'jobs done, starting the last merge step...'
cmd = ['hadd','-f', out]+listf
subprocess.call(cmd)
