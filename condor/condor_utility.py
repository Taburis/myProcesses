
import subprocess
import os

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
