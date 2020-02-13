from WMCore.Configuration import Configuration
config = Configuration()

subScript = "jtcConfig.C"

config.section_("General")
config.section_("Data")
config.section_("JobType")
config.section_("Site")

#config.General.requestName = 'bJTC_bjetMC_GenGen_5TeV_PRIonly_WTAaxis_csvV2p9_17May19'

#------------------config ----------------------
config.JobType.maxJobRuntimeMin =60 
#fileList = 'runTest.txt'
mixing_buffer ='root://eoscms.cern.ch//store/group/phys_heavyions/wangx/PH2018_JetSamples/mixingBuffer/minbias_MC_mixing_buffer.root'
fileList = 'list_MC_PbPb2018_bjet.txt'
#fileList = 'list_Pythia82017pp_bFilter_WTAJetAxis_hiForest.txt'

#config.General.requestName = 'dryrunTest'
config.General.requestName = 'bJTC_bjetMC2018AA_5TeV_test'
#-----------------------------------------------

config.Data.outputPrimaryDataset = 'bJTC_5TeV_pp'
config.General.workArea = 'bJTC_5TeV_pp'
config.General.transferOutputs = True
#config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'PSet.py'
config.JobType.scriptExe = 'runScript_bjtc.sh'
config.JobType.scriptArgs = ['script='+subScript,'buffer_name='+mixing_buffer]
config.JobType.inputFiles = ['FrameworkJobReport.xml',subScript,'lib.tar.gz','.rootlogon.C']

config.Data.userInputFiles = open(fileList).readlines()

#for the case that CMSSW version are not supported version
config.JobType.allowUndistributedCMSSW = True

config.JobType.outputFiles = ['correlation.root']
#config.Site.storageSite = 'T3_US_FNALLPC'
config.Site.storageSite = 'T2_US_Purdue'

config.Site.whitelist = ['T2_US_Purdue']
config.section_("Debug")
#"really" force crab to only run at whitelisted sites
config.Data.ignoreLocality = True
config.Site.ignoreGlobalBlacklist = True
config.Debug.extraJDL = ['+CMS_ALLOW_OVERFLOW=False']


config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = len(config.Data.userInputFiles)

config.Data.outLFNDirBase = '/store/user/wangx/'
config.Data.publication = False