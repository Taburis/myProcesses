
from myProcesses.condor.condor_utility import jobManager

isData = 1


jobname = 'jobDir_MixingMCbuffer'
#jobname = 'btagger_QAs_dijetSample_Reweighted'
dblist = '../dblist/Minbias_MC_list.txt'
exe = 'makeMixSkim.C'
if isData : 
	jobname = 'jobDir_MixingDatabuffer'
	dblist = '../dblist/PbPbData2018_MinBiasFiles.txt'
	exe = 'makeMixSkim_Data.C'

jm = jobManager(jobSite ='cern', 
	jobname = jobname, 	
	method = 'root',
	executable = exe, 
	output_dir='/eos/cms/store/group/phys_heavyions/wangx/mixingBuffer', 
	runlist = dblist,
	time = '8h'
)

jm.nsplit = 1
jm.make_tarball = False
jm.generate_cfg()
jm.submit()
