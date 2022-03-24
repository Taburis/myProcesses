 
import FWCore.ParameterSet.Config as cms
import select_detachedQuadStep_cfi

detachedTripletStepCutSel = select_detachedQuadStep_cfi.detachedQuadStepCutSel.clone(
    src = cms.InputTag("detachedTripletStepTracks"),
)

highPtTripletStepCutBaseTracks = select_detachedQuadStep_cfi.detachedQuadStepCutBaseTracks.clone(
    inputClassifiers = cms.vstring(
		'detachedTripletStepCutSel',
    ),
    trackProducers = cms.VInputTag(
        "detachedTripletStepTracks" )
)

