 
import FWCore.ParameterSet.Config as cms

initialStepCutSel = cms.EDProducer("TrackCutClassifier",
    beamspot = cms.InputTag("offlineBeamSpot"),
    ignoreVertices = cms.bool(False),
    mightGet = cms.optional.untracked.vstring,
    mva = cms.PSet(
        dr_par = cms.PSet(
            d0err = cms.vdouble(0.003, 0.003, 0.003),
            d0err_par = cms.vdouble(0.001, 0.001, 0.001),
            drWPVerr_par = cms.vdouble(3.40282346639e+38, 3.40282346639e+38, 3.40282346639e+38),
            dr_exp = cms.vint32(2147483647, 2147483647, 2147483647),
            dr_par1 = cms.vdouble(3.40282346639e+38, 3.40282346639e+38, 3.40282346639e+38),
            dr_par2 = cms.vdouble(3.40282346639e+38, 3.40282346639e+38, 3.40282346639e+38)
        ),
        dz_par = cms.PSet(
            dzWPVerr_par = cms.vdouble(3.40282346639e+38, 3.40282346639e+38, 3.40282346639e+38),
            dz_exp = cms.vint32(2147483647, 2147483647, 2147483647),
            dz_par1 = cms.vdouble(3.40282346639e+38, 3.40282346639e+38, 3.40282346639e+38),
            dz_par2 = cms.vdouble(3.40282346639e+38, 3.40282346639e+38, 3.40282346639e+38)
        ),
        isHLT = cms.bool(False),
        #maxChi2 = cms.vdouble(0.3, 0.3, 0.3),
        maxChi2 = cms.vdouble(9999.0, 9999.0, 9999.0),
        maxChi2n = cms.vdouble(10.0, 1.0, 0.4),
        maxDr = cms.vdouble(100, 100, 100),
        maxDz = cms.vdouble(100, 100, 100),
        #maxDr = cms.vdouble(3.40282346639e+38, 3.40282346639e+38, 3.40282346639e+38),
        #maxDz = cms.vdouble(3.40282346639e+38, 3.40282346639e+38, 3.40282346639e+38),
        #maxDzWrtBS = cms.vdouble(100, 100, 100),
        #maxDzWrtBS = cms.vdouble(3.40282346639e+38, 24, 15),
        maxLostLayers = cms.vint32(4,3,2),
        maxRelPtErr = cms.vdouble(0.075, 0.075, 0.075),
        min3DLayers = cms.vint32(1,1,1),
        minHits = cms.vint32(0, 5, 8),
        #minHits4pass = cms.vint32(100, 100, 100),
        minHits4pass = cms.vint32(2147483647, 2147483647, 2147483647),
        minLayers = cms.vint32(3,4,5),
        minNVtxTrk = cms.int32(0),
        minNdof = cms.vdouble(-1, -1, -1),
        minPixelHits = cms.vint32(0, 1, 2)
    ),
    qualityCuts = cms.vdouble(-0.7, 0.1, 0.7),
    src = cms.InputTag("initialStepTracks"),
    vertices = cms.InputTag("firstStepPrimaryVertices")
)

initialStepCutBaseTracks= cms.EDProducer("TrackCollectionMerger",
    allowFirstHitShare = cms.bool(True),
    copyExtras = cms.untracked.bool(False),
    copyTrajectories = cms.untracked.bool(False),
    foundHitBonus = cms.double(10),
    inputClassifiers = cms.vstring(
        'initialStepCutSel',
    ),
    lostHitPenalty = cms.double(5),
    mightGet = cms.optional.untracked.vstring,
    minQuality = cms.string('highPurity'),
    minShareHits = cms.uint32(2),
    shareFrac = cms.double(0.19),
    trackAlgoPriorityOrder = cms.string('trackAlgoPriorityOrder'),
    trackProducers = cms.VInputTag(
        "initialStepTracks" )
)
