
from myProcesses.condor.condor_utility import jobManager


jobname = 'job_qaScan_djet'
dblist = '../dblist/list_Hi2018MC_HydjetPythia_drum5tune_QCDJet.txt'
#jobname = 'job_qaScan_data'
#dblist = '../dblist/list_data_hardprob_jet80or100_skim.txt'
#jobname = 'job_qaScan_data'
#dblist = '../dblist/list_bjetMC_skim_cerneos_run2.txt'
#dblist = 'list_bjetMC_run2_temp.txt'
exe = 'scan.C'

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	runlist = dblist,
#	infile = ['cfg.h'],
	time = '1h'
)

jm.nsplit = 50
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
