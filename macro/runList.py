
import subprocess

execf = 'run_bTagger_step1_scan_AA.C'
#listpath = 'DBList/list_2018AA_data_HardProbe_Jet80_Inna.txt'
#output = 'AA2018bTagger_DataJet80_HardprobDB'
listpath = 'DBList/list_2018AA_dijet_PH_Inna.txt'
output = 'AA2018bTagger_dijetMC_HP'
doMerge = 1
listf = open(listpath).readlines()
istart = 0
outputlist = []
for fs in listf :
	outputname = output+'_id'+str(istart)
	outputlist.append(outputname)
	istart+=1
	cmdline = ['root', '-l', '-b', '-q', execf+'(\"'+fs.rstrip()+'\", \"'+outputname+'\")']
	outputlist[-1]=outputlist[-1]+'_step1.root'
	subprocess.call(cmdline)
if doMerge:
	cmdline = ['hadd', '-f', output+'_step1.root']+outputlist
	print cmdline
	subprocess.call(cmdline)
print 'done!'
