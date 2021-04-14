
from myProcesses.condor.condor_utility import jobManager

jobname = 'job_skim'
dblist = 'list_dhanush_pp2017MC_hiForest.txt'
exe = 'scan.C'

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	infile = ['cfg.h'],
	runlist = dblist,
	time = '1h'
)

jm.nsplit = 30
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
