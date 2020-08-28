from CRABClient.UserUtilities import config, getUsernameFromCRIC
config = config()

#config.section_("General")
#config.section_("Data")
#config.section_("JobType")
#config.section_("Site")

config.General.requestName ='RAWHLT_QCD_5TeV_TuneEE_5C_Herwigpp_AOD'
config.General.workArea = 'MC_crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

#config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'step2_DIGI_L1_DIGI2RAW_HLT.py'
config.JobType.maxJobRuntimeMin = 2400
config.JobType.maxMemoryMB = 4000
config.JobType.numCores = 4

#config.Data.outputPrimaryDataset = 'Herwigpp'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.inputDataset = '/Herwigpp/wangx-Herwig_EE_5C_Tune_5TeV_QCD_80to500-df1953e93caa73ff784f829f24bfce6c/USER'
config.Data.inputDBS = 'phys03'
config.Data.outLFNDirBase = '/store/group/phys_heavyions/wangx/' 
config.Data.publication = True
config.Data.outputDatasetTag = 'RAWHLT_AOD_Herwig_EE_5C_Tune_5TeV_QCD_80to500'
#config.Data.outputDatasetTag = 'minBias_HydjetDrum5F_GEN_SIM_PU'
config.JobType.allowUndistributedCMSSW = True

config.Site.storageSite = 'T2_CH_CERN'
