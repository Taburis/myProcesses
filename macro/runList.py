
import subprocess

execf = 'run_bTagger_step1_scan.C'
listpath = 'DBList/list_2018AA_data_HardProbe_Jet80_Inna.txt'
listf = open(listpath).readlines()
output = 'AA2018bTagger_DataJet80_HardprobDB'
doMerge

istart = 0
for fs in listf :
	outputname = output+'_id'+str(istart)
	istart+=1
	cmdline = ['root', '-l', '-b', '-q', execf+'(\"'+fs.rstrip()+'\", \"'+outputname+'\")']
	subprocess.call(cmdline)

cmdline['hadd', '-f', output+'.root', output+'_id*']
subprocess.call(cmdline)
print 'done!'
