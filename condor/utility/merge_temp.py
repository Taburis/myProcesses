
import subprocess


inlist = []
INPUT
output = OUTPUT


for f in listf:
	if not os.path.exists(f): return

cmd = ['hadd','-f', out]+listf
subprocess.call(cmd)
