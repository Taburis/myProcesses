from myProcesses.condor.condor_utility import jobManager

isData = 1

jobname = 'randomConeMC'
#jobname = 'btagger_QAs_dijetSample_pthat60_shift5centOut'
dblist = '../dblist/list_dijet_hiForest.txt'
exe = 'cfg_randomCone.C'
if isData : 
	jobname = 'randomCone_Data'
	dblist = '../dblist/list_data_hardprob_jet80or100_hiForest.txt'
	exe = 'cfg_randomCone_data.C'

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/', 
	runlist = dblist,
	time = '2h'
)

if isData : jm.nsplit = 50
else : jm.nsplit=300
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
