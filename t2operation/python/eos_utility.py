
import os
import sys
import subprocess
import math

def merge(path0, output_dir, eos_prefix):
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

def merge_in_step(name, output, nstep, listf):
	out = []
	total = len(listf)
	njob = int(math.ceil(float(total)/nstep))
	for i in range(njob):
		endindex = (i+1)*nstep
		if endindex > total: endindex = -1 
		cmd = ["hadd", "-f", output+name+"_merge_part%d.root"%i, ]+listf[i*nstep:endindex]
		out.append(output+name+"_merge_part%d.root"%i)
		#subprocess.call(cmd)
	return out

def merge0(name, output,  n, mypath):
	flist = []
	for dirpaths, dirnames, files in os.walk(mypath):
		for f in files:
			if f.split('.')[-1] != 'root': continue
			flist.append(os.path.join(dirpaths, f))
	total = len(flist)
	print 'Find %d root files, will merge them in step of %d'%(total, n)
	if not os.path.exists(output+"tmp/"):
		os.makedirs(output+"tmp/")
	rmlist = []
	final =''
	outname = name
	outlist = merge_in_step(outname, output+"tmp/", n, flist)
	while len(outlist) > 1 : 
		print outlist, '\n'
		rmlist.extend(outlist)
		outname = outname+'_again'
		outlist = merge_in_step(outname, output+"tmp/", n, outlist)
		final=outlist[0]
		print outlist
	#subprocess.call(['rm']+rmlist)
	#subprocess.call(['mv', final, output+name+'.root'])


