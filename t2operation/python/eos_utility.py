
import os
import sys
import subprocess

def merge(path0,output_dir, eos_prefix):
	path = eos_prefix+path0
	if output_dir[-1] != "/": output_dir= output_dir+"/"
	outputs = subprocess.check_output(["ls","-u", path])
	plist = outputs.split("\n")
	print "targeting path:",path
#	doit = raw_input("do you want to merge all .root files ? [y: continue]")
#	if doit != "y" : return;
	if not os.path.exists(output_dir+"tmp/"):
		os.makedirs(output_dir+"tmp/")
	ntotal = 0
	flist = []
	if path[-1] != "/": path = path+"/"
	for line in plist:
		if line[-5: ] == ".root" :
			ntotal = ntotal+1
			flist.append(path+line)
	print("total files: ",ntotal)
	#print(flist)
	fname = raw_input("please tpye output file name or 'break' to stop:")
	if fname == 'break': return	
	commandstring = ["hadd", "-f", output_dir+"tmp/"+fname] + flist
	subprocess.call(commandstring)
	subprocess.call(["mv", output_dir+"tmp/"+fname, output_dir])
	return
	
