
import os
import subprocess
import condor_utility as cu

if __name__ == '__main__' :
	doSubmit = True # set to False for testing the configure

	# cp the files to the local folder, make sure the local folder is large enough
	dumpToEOS = False
	runname = 'pp2017MC_bjet'
	executable = 'jtcConfig_for_condor.C'
	runlist = 'list_Pythia82017pp_bFilter_WTAJetAxis_hiForest.txt'
	#runname = 'PbPb2018DataTrkAna'
	#executable = 'run_data.C'
	#runlist = 'PbPbData2018_newJets_80and100triggers.txt'
	#runlist = 'listTest.txt'
	#runlist = '/afs/cern.ch/user/j/jviinika/public/forXiao/PbPbData2018_newJets_80and100triggers.txt'
	outputname = 'output.root'
	# this is the FNAL t3 eos directory, probably needs to add a purdue t2 directory
	eosdir = 'root://cmseos.fnal.gov//store/user/wangx/CondorJobs/'



	workfolder = runname
	if not os.path.exists('{FOLDER}'.format(FOLDER=workfolder)):
                os.system('mkdir {FOLDER}'.format(FOLDER=workfolder))
                os.system('mkdir {FOLDER}/outCondor'.format(FOLDER=workfolder))

	cmsswDir = os.getenv('CMSSW_BASE')
	pwd = os.getenv('PWD')
	files = open(runlist).readlines()
	counter = 0
	njobs = 0
	for f in files:
		counter+=1
		cmdline = 'root -l -q -b '+executable+'+\'("'+outputname+'_'+str(counter)+'.root","'+f.rstrip()+'")\''
		parlist = {'EXECUTABLE':cmdline}
		parlist['ENV_SETUP'] = 'pushd '+cmsswDir+'/src'
		parlist['PRERUN'] = 'cp '+pwd+'/'+executable+' .\n'+'cp -r '+pwd+'/lib.tar.gz .\n'+'tar -xvf lib.tar.gz'

		outputLocal = 'cp -v '+outputname+'_'+str(counter)+'.root $LS_SUBCWD'
		outputEOS = 'xrdcp -f '+outputname+'_'+str(counter)+'.root '+eosdir+runname+'/'+outputname+'_'+str(counter)+'.root'
		if dumpToEOS: parlist['OUTPUT'] = outputEOS
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
