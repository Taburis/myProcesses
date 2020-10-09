
from myProcesses.condor.condor_utility import jobManager

isData = 0

jobname = 'job_qaScan_data'
dblist = '../dblist/list_data_hardprob_jet80or100_skim_98p.txt'
#dblist = '../dblist/list_bjetMC_skim_cerneos_run2.txt'
#dblist = 'list_bjetMC_run2_temp.txt'
exe = 'scan.C'

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	runlist = dblist,
	infile = ['cfg.h'],
	time = '1h'
)

jm.nsplit = 30
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
