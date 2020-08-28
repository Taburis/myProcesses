from CRABClient.UserUtilities import config, getUsernameFromCRIC
config = config()

#config.section_("General")
#config.section_("Data")
#config.section_("JobType")
#config.section_("Site")

config.General.requestName ='QCD_5TeV_TuneEE_5C_Herwigpp_GEN_SIM'
config.General.workArea = 'MC_crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'QCD_Pt_80to300_TuneEE5C_herwigpp.py'
config.JobType.maxJobRuntimeMin = 2400
config.JobType.maxMemoryMB = 4000
config.JobType.numCores = 2

config.Data.outputPrimaryDataset = 'Herwigpp'
config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 1000
NJOBS = 100  # This is not a configuration parameter, but an auxiliary variable that we use in the next line.
config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
config.Data.outLFNDirBase = '/store/group/phys_heavyions/wangx/' 
config.Data.publication = True
config.Data.outputDatasetTag = 'Herwig_EE_5C_Tune_5TeV_QCD_80to500'
#config.Data.outputDatasetTag = 'minBias_HydjetDrum5F_GEN_SIM_PU'
config.JobType.allowUndistributedCMSSW = True

config.Site.storageSite = 'T2_CH_CERN'
