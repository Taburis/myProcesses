from WMCore.Configuration import Configuration
config = Configuration()

#subScript = "jtcConfig_Data_jeu.C"
subScript = "jtcConfig_Data_nominal.C"

config.section_("General")
config.section_("Data")
config.section_("JobType")
config.section_("Site")

#config.General.requestName = 'bJTC_bjetMC_GenGen_5TeV_PRIonly_WTAaxis_csvV2p9_17May19'

#------------------config ----------------------
config.JobType.maxJobRuntimeMin =360 
#config.General.requestName = 'bjtc_aa2018_hardprobe_data_jet80or100_Uncet_JERSmear20Worse'
#config.General.requestName = 'bjtc_aa2018_hardprobe_data_merge80and100_JECUp'
#config.General.requestName = 'bjtc_aa2018_hardprobe_data_merge80and100_vz13'
config.General.requestName = 'bjtc_aa2018_hardprobe_data_merge80and100_EScheme'
#config.General.requestName = 'bjtc_aa2018_hardprobe_data_merge80and100_3cbin'
#config.General.requestName = 'bjtc_aa2018_hardprobe_data_merge80and100_xcheck'
#config.General.requestName = 'bjtc_aa2018_hardprobe_data_merge80and100_JECref'
#config.General.requestName = 'bjtc_aa2018_hardprobe_data_merge80and100'
#config.General.requestName = 'bjtc_aa2018_hardprobeLower_data_jet80above'
#config.General.requestName = 'bjtc_aa2018_hardprobeLower_data_jet60'

#fileList = '../dblist/list_hi2018HardProbeLower_skim.txt'
fileList = '../dblist/list_HI2018Data_HardProbe_jet80orJet100_skim_goldenJSON.txt'
#fileList = '../dblist/list_HI2018Data_HardProbe_jet80orJet100_skim_goldenJSON_85p.txt'
#fileList = '../dblist/list_data_hardprob_jet80or100_skim.txt'
#fileList = '../dblist/list_data_hardprobe_j80_skim.txt'

minbias_mixing_buffer ='root://eoscms.cern.ch//store/group/phys_heavyions/wangx/mixingBuffer/mixing_buffer_data_minbias_ordered.root'
#-----------------------------------------------

config.Data.outputPrimaryDataset = 'aa2018bjet'
config.General.workArea = 'bjtc_aa5TeV'
config.General.transferOutputs = True
#config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'PSet.py'
config.JobType.scriptExe = 'runScript_bjtc.sh'
config.JobType.scriptArgs = ['script='+subScript,'buffer_name='+minbias_mixing_buffer]
config.JobType.inputFiles = ['FrameworkJobReport.xml',subScript,'lib.tar.gz','.rootlogon.C', 'producerBJTC.h', 'PhiCorrectionGen_AK4PF.txt']

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
