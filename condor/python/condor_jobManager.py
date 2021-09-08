
import math
import subprocess
import os

eos_purdue_t2 = 'root://xrootd.rcac.purdue.edu/'
eos_dir_list ={'cern':'/eos/cms/store/group/phys_heavyions/wangx',
		'purdue':'', '':'', 'lpc':'root://cmseos.fnal.gov//store/user/wangx'}

time_sequence = {'none':'','20m':'"espresso"','1h':'"microcentury"','2h':'"longlunch"','8h':'"workday"','1d':'"tomorrow"','3d':'"testmatch"','1w':'"nextweek"'}

def subText(inf, outf, parlist):
	if len(parlist) == 0:
		print('Error: Empty list abort!')
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
		print('FD exits! Abord!'.format(FD=workfolder))
		return

class jobManager:
	def __init__(self, jobSite, jobname, method, executable, runlist, output_dir='',infile =[], env_mode = 'local', time = '20m', make_tarball = True, stage_out_mode = 'local'):
		self.pwd = os.path.abspath(os.getcwd())
		self.swVersion=os.getenv('CMSSW_VERSION')
		self.swPath=os.getenv('CMSSW_BASE')

		self.jobname = jobname
		self.jobSite = jobSite 
		self.executable = executable
		self.method = method
		self.runlist = runlist
		self.output_dir = output_dir
		self.cfg_template = 'default'
		self.cfg_list =[]
		self.env_mode = env_mode
		self.store_path = eos_dir_list[jobSite]
		self.eos_path = eos_dir_list[jobSite]
		self.time = time
		self.make_tarball = make_tarball
		self.inputFiles =infile # additional files needed
		self.nsplit = 1
		self.binary = ''
		self.stageOutMode = stage_out_mode

		#	def set_env_prefix(self):
		#		env = 'export PYTHONHOME="/cvmfs/cms.cern.ch/slc7_amd64_gcc820/external/python/2.7.15/"\n'	
		#		return env

	def set_run_environment_local(self):
		ver = os.getenv('CMSSW_VERSION')
		base= os.getenv('CMSSW_BASE')
		cmd = 'pushd '+base+'/src\n'+'eval `scramv1 runtime -csh`\npushd '
		cmd = cmd+pwd+'/'+self.jobname+'\n'
		return cmd

	def set_run_environment_tarball(self):
		cmssw_ver = self.swVersion
		cmd = 'mkdir '+cmssw_ver+'\npushd '+cmssw_ver+'\nxrdcp -f '+self.eos_path+'/condorJob/'+self.jobname+'/'+cmssw_ver+'.tgz ./\ntar -xf '+cmssw_ver+'.tgz\nrm '+cmssw_ver+'.tgz\npushd src\nscramv1 b ProjectRename\n' 
		#print( self.swPath)
		#print( self.pwd.split(self.swPath))
		path = self.pwd.split(self.swPath+'/src/')[1]+'/'+self.jobname
		cmd = cmd+'eval `scramv1 runtime -csh`\n'
		cmd = cmd+'pushd ./'+path
		return cmd

	def set_run_environment(self):
		if self.env_mode =='tarball': return self.set_run_environment_tarball()
		elif self.env_mode == 'local': return self.set_run_environment_local()

	def set_run_time(self):
			time_s = '+JobFlavour = '+time_sequence[self.time]+'\n'
			return time_s

	def jobSplit(self):
		files = open(self.runlist).readlines()
		njobs = int(math.ceil(float(len(files))/self.nsplit))
		split_list=[]
		for i in range(njobs):
			joblist = []
			starti=i*self.nsplit
			endi = (i+1)*self.nsplit
			for f in files[starti:endi]:
				joblist.append(f)
			split_list.append(joblist)
		return split_list

	def stageOut_local(self, infile):
		fname = infile.split('/')[-1]
		return 'mv '+infile+' ./data/'+fname

	def stageOut_eos(self, infile):
		fname = infile.split('/')[-1]
		return 'xrdcp '+infile+' '+self.eos_path+'/condorJob/'+self.jobname+'/'+fname
	
	def stageOut_cmd(self, infile):
		if self.stageOutMode == 'local': return self.stageOut_local(infile)
		elif self.stageOutMode == 'eos': return self.stageOut_eos(infile)

	def packArg(self, ijob, lists, method='root'):
		inputs = lists[ijob]
		if(method == 'root'):
			cmdline = ''
			for i in range(len(inputs)):
				f = inputs[i]
				outputname = './temp'+str(ijob)+'/job_output'
				cmdline = cmdline+self.binary+' '+self.executable+'\'("'+f.rstrip()+'","'+outputname+'_'+str(i)+'.root")\'\n'
			if len(inputs) > 1: 
				cmdline = cmdline+'hadd -f ./temp'+str(ijob)+'/'+'job_output_part'+str(ijob)+'.root ./temp'+str(ijob)+'/*.root\n'
			else :
				cmdline = cmdline+'mv ./temp'+str(ijob)+'/job_output_0.root ./temp'+str(ijob)+'/job_output_part'+str(ijob)+'.root'
				
			return './temp'+str(ijob)+'/'+'job_output_part'+str(ijob)+'.root', cmdline

	def generate_cfg(self):
		# make a workfing folder to contain the configs
		workfolder = self.jobname
		cmsswDir = os.getenv('CMSSW_BASE')
		if not os.path.exists('{FOLDER}'.format(FOLDER=workfolder)):
			os.system('mkdir {FOLDER}'.format(FOLDER=workfolder))
			os.system('mkdir {FOLDER}/outCondor'.format(FOLDER=workfolder))
			os.system('mkdir {FOLDER}/data'.format(FOLDER=workfolder))
			os.system('cp -v {exe} {FOLDER}/'.format(FOLDER=workfolder,exe=self.executable))
		for files in self.inputFiles:
			os.system('cp -v {exe} {FOLDER}/'.format(FOLDER=workfolder,exe=files))


		if self.method == 'root': self.binary = 'root -b -l -q'	
		elif self.method == 'cmsRun': self.binary = 'cmsRun'
		pwd = os.getenv('PWD')
		files = open(self.runlist).readlines()
		prerun_cmd = ''
		env_setup = 'source /cvmfs/cms.cern.ch/cmsset_default.sh\n'
		env_setup = env_setup+self.set_run_environment()
		if self.jobSite == 'cern' or self.jobSite == 'lpc':
			script_temp = cmsswDir+'/src/myProcesses/condor/utility/script_condor_template.sh'
			cfg_temp = cmsswDir+'/src/myProcesses/condor/utility/condor_template.cfg'
		ifiles = 0
		outputname=''
		flist = self.jobSplit()
		for i in range(len(flist)):
			prerun_cmd = 'mkdir temp'+str(i)+'\n'
			output, cmdline = self.packArg(i, flist, self.method)
			output_cmd = self.stageOut_cmd(output) 
			parlist = {'EXECUTABLE':cmdline}
			parlist['ENV_SETUP'] = env_setup
			parlist['PRERUN'] = prerun_cmd
			parlist['OUTPUT'] = output_cmd
			subText(script_temp, workfolder+'/script_'+str(i)+'.sh', parlist)
			parlist = {}
			parlist['SCRIPT'] = 'script_'+str(i)+'.sh'
			os.system('chmod 755 '+workfolder+'/script_'+str(i)+'.sh')
			parlist['KINDEX'] = str(i)
			parlist['FOLDER'] = pwd+'/'+workfolder
			parlist['TIME'] = self.set_run_time()
			subText(cfg_temp, workfolder+'/condor_cfg_'+str(i)+'.cfg', parlist)
			self.cfg_list.append('condor_cfg_'+str(i)+'.cfg')
		if self.make_tarball : 
			subprocess.call(['tar', '-zvcf', self.swVersion+'.tgz','-C', self.swPath, '.'])
			subprocess.call(['xrdcp', '-p','-f',self.swVersion+'.tgz', self.eos_path+'/condorJob/'+self.jobname+'/'+self.swVersion+'.tgz'])
			subprocess.call(['rm',self.swVersion+'.tgz'])
	def submit(self):
		os.chdir(self.jobname)
		for cfg in self.cfg_list:
#			os.system("echo {CFG}".format(CFG=cfg)) 
			os.system("condor_submit {CFG}".format(CFG=cfg)) 
 
