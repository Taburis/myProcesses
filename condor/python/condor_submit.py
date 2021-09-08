import os
import argparse

parser = argparse.ArgumentParser()

parser.add_argument('-p', dest='path', help='path to search')
parser.add_argument('-c', dest='cfg', help='config')
args = parser.parse_args()
inpath = args.path
cfg = args.cfg

os.chdir(inpath)

flist = []
files = os.listdir(inpath)

for f in files:
	if f.split(cfg)[0] != '':
		continue
	flist.append(f)

for f in flist:
	os.system("condor_submit {CFG}".format(CFG=f))
