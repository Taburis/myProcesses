
from myProcesses.condor.condor_utility import jobManager

isData = 0


jobname = 'jet80andAbove'
#jobname = 'btagger_QAs_dijetSample_Reweighted'
dblist = 'DBList/list_2018AA_dijet_PH_Inna.txt'
exe = 'run_bTaggerStep1Config.C'
if isData : 
	jobname = 'btagger_step1_data_Jet80'
	dblist = 'DBList/list_2018AA_data_HardProbe_Jet80_Inna.txt'
	exe = 'run_bTaggerStep1Config_Data.C' 

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	runlist = dblist,
	time = '1h'
)

jm.nsplit = 80
jm.make_tarball = False
jm.generate_cfg()
jm.submit()