from myProcesses.condor.condor_utility import jobManager

isData = 0


jobname = 'jobDir_orderedBuff'
#jobname = 'btagger_QAs_dijetSample_Reweighted'
dblist = 'list_minbias.txt'
exe = 'ordered_buffer.C'
if isData : 
	jobname = 'jobDir_orderedBuff'
	dblist = 'list_minbias_data.txt'

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
