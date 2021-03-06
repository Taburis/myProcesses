
from myProcesses.condor.condor_utility import jobManager


jobname = 'job_qaScan_jesmeared'
dblist = './list_PYTHIA6_pthat80_hiForest_purdue.txt'
#dblist = './list_PYTHIA6_hiForest_purdue.txt'
#jobname = 'job_qaScan_data'
#dblist = '../dblist/list_data_hardprob_jet80or100_skim.txt'
#jobname = 'job_qaScan_bjet_btagging'
#dblist = '../dblist/list_Hi2018MC_HydjetPythia_drum5tune_bJet.txt'
exe = 'scan_jesmeared.C'
#exe = 'scan.C'

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	runlist = dblist,
	infile = ['cfg.h'],
	time = '20m'
)

jm.nsplit = 8
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
