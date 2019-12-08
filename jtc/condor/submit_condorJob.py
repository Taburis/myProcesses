
import os
import subprocess
import condor_utility as cu

if __name__ == '__main__' :
	runname = 'PbPb2018MCTrkAna'
	executable = 'run_mc.C'
	runlist = 'PbPbMC2018_commonForest.txt'
	outputname = 'output'
	eos_purdue = 'root://xrootd.rcac.purdue.edu//store/user/wangx/condor/'

	workfolder = runname
	if not os.path.exists('{FOLDER}'.format(FOLDER=workfolder)):
                os.system('mkdir {FOLDER}'.format(FOLDER=workfolder))
                os.system('mkdir {FOLDER}/outCondor'.format(FOLDER=workfolder))

	cmsswDir = os.getenv('CMSSW_BASE')
	pwd = os.getenv('PWD')
	files = open(runlist).readlines()

	cmdPre cu.makeTdrBall(eos_path)
	counter = 0
	njobs = 0
	for f in files:
		counter+=1
		cmdline = 'root -l -q -b '+executable+'\'("'+outputname+'_'+str(counter)+'.root","'+f.rstrip()+'")\''
		parlist = {'EXECUTABLE':cmdline}
		parlist['ENV_SETUP'] = cmdPre
		parlist['PRERUN'] = 'cp '+pwd+'/'+executable+' .\n'+'cp -r '+pwd+'/lib .'
		parlist['OUTPUT'] = 'cp -v '+outputname+'_'+str(counter)+'.root $LS_SUBCWD'
		cu.subText('script_condor_template.sh', workfolder+'/script_'+str(counter)+'.sh', parlist)
		parlist = {}
		parlist['SCRIPT'] = 'script_'+str(counter)+'.sh'
		parlist['KINDEX'] = str(counter)
		parlist['FOLDER'] = pwd+'/'+workfolder
		cu.subText('condor_template.cfg', workfolder+'/condor_cfg_'+str(counter)+'.cfg', parlist)
	os.chdir(workfolder)
	for i in xrange(1, counter+1):
		cfgpath = 'condor_cfg_'+str(i)+'.cfg' 
		os.system("condor_submit {CFG}".format(CFG=cfgpath))
"""
