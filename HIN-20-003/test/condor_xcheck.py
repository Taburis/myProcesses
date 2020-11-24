
from myProcesses.condor.condor_utility import jobManager

isData = 0

jobname = 'job_xcheck_newsample20m'
#jobname = 'job_xcheck_Inna'
dblist = 'list_Hi2018MC_HydjetPythia_drum5tune_bJet.txt'
#dblist = 'list_Inna.txt'
exe = 'xcheck.C'

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	runlist = dblist,
	infile = ['cfg.h'],
	time = '20m'
)

jm.nsplit = 30
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
