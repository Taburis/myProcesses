
import os
import subprocess
import sys
sys.path.insert(1,'../')
import condor_utility as cu

if __name__ == '__main__' :
	doSubmit = False # set to False for testing the configure

	# cp the files to the local folder, make sure the local folder is large enough
	dumpToEOS = True
	runname = 'pp2017MC_bjet'
	executable = 'jtcConfig_for_condor.C'
	runlist = 'list_Pythia82017pp_bFilter_WTAJetAxis_hiForest.txt'
	eos_site = cu.eos_purdue_t2
	eos_path = eos_site+'/store/user/wangx/condor/'
	outputname = 'output' # don't add .root
	

	workfolder = runname
	if not os.path.exists('{FOLDER}'.format(FOLDER=workfolder)):
                os.system('mkdir {FOLDER}'.format(FOLDER=workfolder))
                os.system('mkdir {FOLDER}/outCondor'.format(FOLDER=workfolder))

	cmsswDir = os.getenv('CMSSW_BASE')
	ver = os.getenv('CMSSW_VERSION')
	pwd = os.getenv('PWD')
	sub_dir = pwd[pwd.find('src'):]
	cmdPre = cu.makeTdrBall(eos_path)
	files = open(runlist).readlines()
	counter = 0
	njobs = 0
	for f in files:
		counter+=1
		cmdline = 'root -l -q -b '+executable+'+\'("'+outputname+'_'+str(counter)+'.root","'+eos_site+f.rstrip()+'")\'\npopd'
		parlist = {'EXECUTABLE':cmdline}
		parlist['ENV_SETUP'] = cmdPre
		parlist['PRERUN'] = 'pushd '+sub_dir

		outputEOS = 'xrdcp -f '+outputname+'_'+str(counter)+'.root '+eos_path+runname+'/'+outputname+'_'+str(counter)+'.root'
		if dumpToEOS: parlist['OUTPUT'] = outputEOS+'\npopd\nrm -rf '+ver
		else :  parlist['OUTPUT'] = outputLocal
		cu.subText('script_condor_template.sh', workfolder+'/script_'+str(counter)+'.sh', parlist)
		parlist = {}
		parlist['SCRIPT'] = 'script_'+str(counter)+'.sh'
		parlist['KINDEX'] = str(counter)
		parlist['FOLDER'] = pwd+'/'+workfolder
		cu.subText('condor_template.cfg', workfolder+'/condor_cfg_'+str(counter)+'.cfg', parlist)
	os.chdir(workfolder)
	print 'configs has been generated!'
	for i in xrange(1, counter+1):
		cfgpath = 'condor_cfg_'+str(i)+'.cfg' 
		if doSubmit : 
			if i%10 == 0 : print str(i)+' jobs have been submitted...'
			os.system("condor_submit {CFG}".format(CFG=cfgpath))
	if doSubmit: print 'submition has been done!'
