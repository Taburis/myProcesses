
from myProcesses.condor.condor_utility import jobManager

jm = jobManager(jobSite ='cern', 
	jobname = 'btagger_step1_dijetSample', 	
	method = 'root',
	executable = 'run_bTaggerStep1Config.C', 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	runlist = './DBList/list_2018AA_dijet_PH_Inna.txt',
	time = '20m'
)

jm.nsplit = 100
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
