# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step3 --customise SLHCUpgradeSimulations/Configuration/combinedCustoms.cust_2017 --conditions auto:phase1_2017_design -s RAW2DIGI,L1Reco,RECO --datatier GEN-SIM-RECO -n 10 --geometry Extended2017 --eventcontent FEVTDEBUGHLT --no_exec --filein file:step2.root --fileout file:step3.root
import FWCore.ParameterSet.Config as cms
import sys
from Configuration.StandardSequences.Eras import eras

from Configuration.Eras.Era_Run3_pp_on_PbPb_cff import Run3_pp_on_PbPb
from Configuration.Eras.Modifier_pp_on_PbPb_run3_cff import pp_on_PbPb_run3


process = cms.Process('RECO',Run3_pp_on_PbPb)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring("file:/eos/cms/store/group/phys_heavyions/wangx/EmbeddedSample/embeddingSample_QCD_Pthat_80_120_5TeV_TuneCP5_HydjetDrum5F_GEN_SIM_PU_11_2_0_pre8_Condition/201122_180228/0000/step1_DIGI_L1_DIGI2RAW_HLT_PU_1.root"),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step3 nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RECO'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(1048576),
    fileName = cms.untracked.string('file:TTBarWithRelVal.root'),
    outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

process.load("SimGeneral.TrackingAnalysis.trackingParticles_cfi")
#process.load("DQMServices.Components.EDMtoMEConverter_cff")

usePhase1 = True

process.load("SimTracker.TrackAssociatorProducers.quickTrackAssociatorByHits_cfi")

# Additional output definition
process.myAnalyzer = cms.EDAnalyzer("trackingMVA_skimer",
				    isPhase1 = cms.bool(usePhase1),
				    makeMVATree_=cms.bool(True),
				    makeSimTree_=cms.bool(True),
				    source=cms.string("generalTracks"),
                                    simSource=cms.InputTag("mix","MergedTrackTruth"),
                                    beamspot = cms.InputTag("offlineBeamSpot"),
                                    vertices = cms.InputTag("firstStepPrimaryVertices"),
                                    outfile=cms.string('output.root'),
                                    associator=cms.string("quickTrackAssociatorByHits"),
                                    )

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic_hi', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction*process.tpClusterProducer*process.quickTrackAssociatorByHits*process.myAnalyzer)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.endjob_step)


# customisation of the process.

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.combinedCustoms
#from SLHCUpgradeSimulations.Configuration.combinedCustoms import cust_2017 

#call to customisation function cust_2017 imported from SLHCUpgradeSimulations.Configuration.combinedCustoms
#process = cust_2017(process)


# End of customisation functions
doCutBaseTracking = False
if doCutBaseTracking : 
	print("WARNING: RECO with the cutbased filters...")
	process.load("extra_producer")
	pp_on_PbPb_run3.toModify(process.initialStep, src = cms.InputTag("initialStepCutBaseTracks"))
	pp_on_PbPb_run3.toModify(process.highPtTripletStep, src = cms.InputTag("highPtTripletStepCutBaseTracks"))
	pp_on_PbPb_run3.toModify(process.lowPtTripletStep, src = cms.InputTag("lowPtTripletStepCutBaseTracks"))
	pp_on_PbPb_run3.toModify(process.lowPtQuadStep, src = cms.InputTag("lowPtQuadStepCutBaseTracks"))
	pp_on_PbPb_run3.toModify(process.earlyGeneralTracks, 
		trackProducers = cms.VInputTag(
			"initialStepCutBaseTracks", "highPtTripletStepCutBaseTracks", 
			"jetCoreRegionalStepTracks", "lowPtQuadStepCutBaseTracks", 
			"lowPtTripletStepCutBaseTracks", "detachedQuadStepTracks", 
			"detachedTripletStepTracks", "pixelPairStepTracks", 
			"mixedTripletStepTracks", "pixelLessStepTracks","tobTecStepTracks"
	    )
	)
	
	process.InitialStepTask = cms.Task(process.caloJetsForTrkTask, process.firstStepPrimaryVertices, process.firstStepPrimaryVerticesUnsorted, process.initialStepCutSel, process.initialStep, process.initialStepClassifier1, process.initialStepHitDoublets, process.initialStepHitQuadruplets, process.initialStepSeedLayers, process.initialStepSeeds, process.initialStepTrackCandidates, process.initialStepTrackRefsForJets, process.initialStepTrackingRegions, process.initialStepTracks, process.initialStepCutBaseTracks)
	
	process.HighPtTripletStepTask = cms.Task(process.highPtTripletStepCutSel, process.highPtTripletStep, process.highPtTripletStepClusters, process.highPtTripletStepHitDoublets, process.highPtTripletStepHitTriplets, process.highPtTripletStepSeedLayers, process.highPtTripletStepSeeds, process.highPtTripletStepTrackCandidates, process.highPtTripletStepTrackingRegions, process.highPtTripletStepTracks, process.highPtTripletStepCutBaseTracks)
	
	process.LowPtQuadStepTask = cms.Task(process.lowPtQuadStepCutSel,process.lowPtQuadStep, process.lowPtQuadStepClusters, process.lowPtQuadStepHitDoublets, process.lowPtQuadStepHitQuadruplets, process.lowPtQuadStepSeedLayers, process.lowPtQuadStepSeeds, process.lowPtQuadStepTrackCandidates, process.lowPtQuadStepTrackingRegions, process.lowPtQuadStepTracks, process.lowPtQuadStepCutBaseTracks)
	
	process.LowPtTripletStepTask = cms.Task(process.lowPtTripletStepCutSel, process.lowPtTripletStep, process.lowPtTripletStepClusters, process.lowPtTripletStepHitDoublets, process.lowPtTripletStepHitTriplets, process.lowPtTripletStepSeedLayers, process.lowPtTripletStepSeeds, process.lowPtTripletStepTrackCandidates, process.lowPtTripletStepTrackingRegions, process.lowPtTripletStepTracks, process.lowPtTripletStepCutBaseTracks)

