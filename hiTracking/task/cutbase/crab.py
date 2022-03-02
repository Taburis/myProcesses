from CRABClient.UserUtilities import config, getUsernameFromCRIC
config = config()

#config.section_("General")
#config.section_("Data")
#config.section_("JobType")
#config.section_("Site")

config.General.requestName ='trkMVA_ntuple_generalTk_baseline'
#config.General.requestName ='trkMVA_ntuple_generalTk_baseline'
config.General.workArea = 'jobStatus'
config.General.transferOutputs = True
config.General.transferLogs = False

#config.JobType.pluginName = 'PrivateMC'
#config.JobType.psetName = 'runTuple_crab.py'
config.JobType.psetName = 'runTuple_nominal.py'
config.JobType.outputFiles = ['output.root']
config.JobType.inputFiles = ['extra_producer.py']
#config.JobType.maxJobRuntimeMin = 2400
config.JobType.maxMemoryMB = 4000
config.JobType.numCores = 1

#config.Data.outputPrimaryDataset = 'MinBias'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
NJOBS = 100  # This is not a configuration parameter, but an auxiliary variable that we use in the next line.
config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
config.Data.publication = False
config.Data.outLFNDirBase= '/store/group/phys_heavyions/wangx/' 
config.Data.inputDataset = '/MB_Hydjet_PbPb5p02TeV_GENSIM_Run3Cond_CMSSW_12_1_0_patch3_v2/jaebeom-MB_Hydjet_PbPb5p02TeV_DIGI2RAW_Run3_CMSSW_12_1_0_patch3_v1-d014be161689e4951b95ad13e9cb33e0/USER'
#config.Data.inputDataset = '/HiTrackingDev2021/wangx-step2_minbias_HydjetDrum5F_cmssw12_2_0_pre2-1a5301505b48ca0ac7b8c18f8c8786a8/USER'
config.Data.inputDBS = 'phys03'
config.Data.outputDatasetTag = config.General.requestName
config.JobType.allowUndistributedCMSSW = True

config.Site.storageSite = 'T2_CH_CERN'
