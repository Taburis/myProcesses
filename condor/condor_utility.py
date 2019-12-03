
import subprocess
import os

eos_purdue_t2 = 'root://xrootd.rcac.purdue.edu/'

def makeTdrBall(eospath):
	cmssw_dir = os.getenv('CMSSW_BASE')
	cmssw_ver = os.getenv('CMSSW_VERSION')
	subprocess.call(['tar', '-zvcf', cmssw_ver+'.tgz','-C', cmssw_dir, '.'])
	subprocess.call(['xrdcp', '-f',cmssw_ver+'.tgz', eospath+cmssw_ver+'.tgz'])
	return 'mkdir '+cmssw_ver+'\npushd '+cmssw_ver+'\nxrdcp -f '+eospath+cmssw_ver+'.tgz ./\ntar -xf '+cmssw_ver+'.tgz\npushd src\n' 

def subText(inf, outf, parlist):
        if len(parlist) == 0:
                print 'Error: Empty list abort!'
                return
        ff = open(inf, 'r')
        lines = ff.readlines()
        of = open(outf, 'w+')
        newlines = []
        for line in lines:
                newline = line
                for k in parlist.keys() :
                        newline = newline.replace(k, str(parlist[k]), 1)
                newlines.append(newline)
        of.writelines(newlines)
	of.close()

def mkdirCheck(workfolder):
        if not os.path.exists('{FOLDER}'.format(FOLDER=workfolder)):
                os.system('mkdir -p -v {FOLDER}'.format(FOLDER=workfolder))
        else :
                print 'FD exits! Abord!'.format(FD=workfolder)
                return
