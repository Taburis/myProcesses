
from myProcesses.condor.condor_utility import jobManager

isData = 0

#jobname = 'btagger_QAs_dijetSample_pthat60_jtptWeighted_forRAA'
jobname = 'btagger_QAs_dijetSample_pthat60_jtptWeighted'
#jobname = 'btagger_QAs_dijetSample_pthat60_shift5centOut'
dblist = 'DBList/list_2018AA_dijet_PH_Inna.txt'
exe = 'run_bTaggerStep1Config.C'
if isData : 
	jobname = 'btagger_QAs_data_muonEnrich'
	#jobname = 'btagger_QAs_data_Jet80_forRAA'
	#jobname = 'btagger_QAs_data_Jet80_shift5pcentOut'
	dblist = 'DBList/list_2018AA_data_HardProbe_Jet80_Inna.txt'
	exe = 'run_bTaggerStep1Config_Data.C' 

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	runlist = dblist,
	infile = ['weight_inclJetPtWeight.root'],
	time = '1h'
)

jm.nsplit = 1
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
