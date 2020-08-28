from CRABClient.UserUtilities import config, getUsernameFromCRIC
config = config()

#config.section_("General")
#config.section_("Data")
#config.section_("JobType")
#config.section_("Site")

config.General.requestName ='RECO_QCD_5TeV_TuneEE_5C_Herwigpp_AOD'
config.General.workArea = 'MC_crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

#config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'step3_RAW2DIGI_L1Reco_RECO_RECOSIM.py'
config.JobType.maxJobRuntimeMin = 2400
config.JobType.maxMemoryMB = 4000
config.JobType.numCores = 8

#config.Data.outputPrimaryDataset = 'Herwigpp'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.inputDataset = '/Herwigpp/wangx-RAWHLT_AOD_Herwig_EE_5C_Tune_5TeV_QCD_80to500-80b61592f1d146ba2cec037d791a9812/USER'
config.Data.inputDBS = 'phys03'
config.Data.outLFNDirBase = '/store/group/phys_heavyions/wangx/' 
config.Data.publication = True
config.Data.outputDatasetTag = 'RECO_AOD_Herwig_EE_5C_Tune_5TeV_QCD_80to500'
#config.Data.outputDatasetTag = 'minBias_HydjetDrum5F_GEN_SIM_PU'
config.JobType.allowUndistributedCMSSW = True

config.Site.storageSite = 'T2_CH_CERN'
