from WMCore.Configuration import Configuration
config = Configuration()

subScript = "jtcConfig_MC.C"

config.section_("General")
config.section_("Data")
config.section_("JobType")
config.section_("Site")

#config.General.requestName = 'bJTC_bjetMC_GenGen_5TeV_PRIonly_WTAaxis_csvV2p9_17May19'

#------------------config ----------------------
config.JobType.maxJobRuntimeMin =600 
#config.General.requestName = 'bjtc_aa2018_bjetP8H_drum_format2_std_run1'
#config.General.requestName = 'bjtc_aa2018_bjetP8H_drum_format2_sube_c5shift_q50'
config.General.requestName = 'bjtc_aa2018_bjetP8H_drum_format2_sube_c5shift_q50_cwfix'
#config.General.requestName = 'bjtc_aa2018_bjetP8H_drum_format2_std_c5shift_q50_cwfix'
#config.General.requestName = 'bjtc_aa2018_bjetP8H_drum_format2_std_c5shift_q50'
#config.JobType.maxMemoryMB = 4000
#config.General.requestName = 'bjtc_aa2018_bjetP8H_drum_format2_sube_run1'


fileList = '../dblist/list_bjetMC_skim_purdue_run1.txt'

#minbiasMC_mixing_buffer ='root://eoscms.cern.ch//store/group/phys_heavyions/wangx/PH2018_JetSamples/mixingBuffer/minbias_MC_mixing_buffer.root'
#minbiasMC_mixing_buffer ='root://eoscms.cern.ch//store/group/phys_heavyions/wangx/PH2018_JetSamples/mixingBuffer/mixing_buffer_ordered_Vz60_C180.root'

minbiasMC_mixing_buffer ='root://eoscms.cern.ch//store/group/phys_heavyions/wangx/mixingBuffer/mixing_buffer_MC_ordered_sube_vz60_hi180.root'
mixing_buffer = minbiasMC_mixing_buffer
#-----------------------------------------------

config.Data.outputPrimaryDataset = 'aa2018bjet'
config.General.workArea = 'bjtc_aa5TeV'
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
#config.Site.storageSite = 'T2_US_Purdue'
#config.Data.outLFNDirBase = '/store/user/wangx/'
config.Site.storageSite = 'T2_CH_CERN'
config.Data.outLFNDirBase = '/store/group/phys_heavyions/wangx/'

config.section_("Debug")
#"really" force crab to only run at whitelisted sites
#config.Site.whitelist = ['T2_US_Purdue']
#config.Data.ignoreLocality = True
#config.Site.ignoreGlobalBlacklist = True
#config.Debug.extraJDL = ['+CMS_ALLOW_OVERFLOW=False']


config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = len(config.Data.userInputFiles)

config.Data.publication = False
