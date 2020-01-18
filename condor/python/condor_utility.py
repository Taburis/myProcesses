
import math
import subprocess
import os

eos_purdue_t2 = 'root://xrootd.rcac.purdue.edu/'
eos_dir_list ={'cern':'/eos/cms/store/group/phys_heavyions/wangx',
	'purdue':''}

time_sequence = {'none':'','20m':'espresso','1h':'microcentury','2h':'longlunch','8h':'workday','1d':'tomorrow','3d':'testmatch','1w':'nextweek'}

def makeTdrBall(eospath, make = False):
	eospath = eospath+'tarball/'
	cmssw_dir = os.getenv('CMSSW_BASE')
	cmssw_ver = os.getenv('CMSSW_VERSION')
	if make:
		subprocess.call(['tar', '-zvcf', cmssw_ver+'.tgz','-C', cmssw_dir, '.'])
		subprocess.call(['xrdcp', '-p','-f',cmssw_ver+'.tgz', eospath+cmssw_ver+'.tgz'])
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

class jobManager:
        def __init__(self, jobSite, jobname, method, executable, runlist, output_dir, env_mode = 'local', time = '20m'):
		self.jobname = jobname
                self.jobSite = jobSite 
                self.executable = executable
		self.method = method
                self.runlist = runlist
                self.output_dir = output_dir
                self.cfg_template = 'default'
		self.cfg_list =[]
		self.env_mode = env_mode
		self.make_tarball = False 
		self.store_path = eos_dir_list[jobSite]
		self.time = '20m'
		self.nsplit = 1
	def set_env_prefix(self):
		env = 'export PYTHONHOME="/cvmfs/cms.cern.ch/slc7_amd64_gcc820/external/python/2.7.15/"\n'	
		return env

	def set_run_environment_local(self):
	        pwd = os.getenv('PWD')
		ver = os.getenv('CMSSW_VERSION')
		base= os.getenv('CMSSW_BASE')
		cmd = 'pushd '+base+'/src\n'+'eval `scramv1 runtime -csh`\npushd '
		cmd = cmd+pwd+'/'+self.jobname+'\n'
		return cmd

	def set_run_environment_tarball(self):
		path = self.store_path
		cmd = makeTdrBall(path, self.make_tarball)
	        pwd = os.getenv('PWD')
		ver = os.getenv('CMSSW_VERSION')
		cmd = cmd+'eval `scramv1 runtime -csh`\n'
	 	working_dir = pwd.split(ver)
		cmd = cmd+'popd\n'
		cmd = cmd+'pushd .'+working_dir[1]
		return cmd

	def set_run_environment(self):
		if self.env_mode =='tarball': return self.set_run_environment_tarball()
		elif self.env_mode == 'local': return self.set_run_environment_local()
		
	def set_run_time(self):
		time_s = '+JobFlavour = "'+time_sequence[self.time]+'"\n'
		return time_s

        def generate_cfg(self):
		nsplit=self.nsplit
		workfolder = self.jobname
	        if not os.path.exists('{FOLDER}'.format(FOLDER=workfolder)):
	                os.system('mkdir {FOLDER}'.format(FOLDER=workfolder))
	                os.system('mkdir {FOLDER}/outCondor'.format(FOLDER=workfolder))
	                os.system('mkdir {FOLDER}/output'.format(FOLDER=workfolder))
	                os.system('cp -v {exe} {FOLDER}/'.format(FOLDER=workfolder,exe=self.executable))

		if self.method == 'root': self.method = 'root -b -l -q'	
	        cmsswDir = os.getenv('CMSSW_BASE')
	        pwd = os.getenv('PWD')
	        files = open(self.runlist).readlines()
		outputname = 'job_output'
		file_keep = ''
	        njobs = 0
		counter = 0
		njobs = int(math.ceil(float(len(files))/nsplit))
		prerun_cmd = ''
		env_setup = self.set_env_prefix()
		env_setup = env_setup+self.set_run_environment()
		if self.jobSite == 'cern':
			script_temp = cmsswDir+'/src/myProcesses/condor/utility/script_condor_template.sh'
			cfg_temp = cmsswDir+'/src/myProcesses/condor/utility/condor_template.cfg'
	        for i in range(njobs):
			if nsplit !=1 : 
				outputname = './temp'+str(i)+'/'+'job_output'
				prerun_cmd = 'mkdir temp'+str(i)+'\n'
	                counter+=1
			starti=i*nsplit
			endi = (i+1)*nsplit
			cmdline = ''
			ifiles = 0
	        	for f in files[starti:endi]:
	                	cmdline = cmdline+self.method+' '+self.executable+'\'("'+f.rstrip()+'","'+outputname+'_'+str(ifiles)+'.root")\'\n'
				ifiles = ifiles+1
			if nsplit !=1 : 
				file_keep='job_output_part'+str(i)+'.root'
				cmdline = cmdline+'hadd -f '+file_keep+' ./temp'+str(i)+'/*.root\n'
				cmdline = cmdline+'rm -r temp'+str(i)+'\n'
			else : file_keep = outputname+str(i)+'.root'
	                parlist = {'EXECUTABLE':cmdline}
	                parlist['ENV_SETUP'] = env_setup
	                parlist['PRERUN'] = prerun_cmd
	                parlist['OUTPUT'] = 'cp -v '+file_keep+' $LS_SUBCWD'+'/output/'+file_keep
	                subText(script_temp, workfolder+'/script_'+str(counter)+'.sh', parlist)
	                parlist = {}
	                parlist['SCRIPT'] = 'script_'+str(counter)+'.sh'
			os.system('chmod 755 '+workfolder+'/script_'+str(counter)+'.sh')
	                parlist['KINDEX'] = str(counter)
	                parlist['FOLDER'] = pwd+'/'+workfolder
	                parlist['TIME'] = self.set_run_time()
	                subText(cfg_temp, workfolder+'/condor_cfg_'+str(counter)+'.cfg', parlist)
			self.cfg_list.append('condor_cfg_'+str(counter)+'.cfg')
        def submit(self):
		os.chdir(self.jobname)
	        for cfg in self.cfg_list:
#			os.system("echo {CFG}".format(CFG=cfg)) 
			os.system("condor_submit {CFG}".format(CFG=cfg)) 

