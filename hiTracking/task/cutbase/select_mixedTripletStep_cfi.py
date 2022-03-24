 
import FWCore.ParameterSet.Config as cms
import select_detachedQuadStep_cfi

mixedTripletStepCutSel = select_detachedQuadStep_cfi.detachedQuadStepCutSel.clone(
    src = cms.InputTag("mixedTripletStepTracks"),
)

mixedTripletStepCutBaseTracks = select_detachedQuadStep_cfi.detachedQuadStepCutBaseTracks.clone(
    inputClassifiers = cms.vstring(
		'mixedTripletStepCutSel',
    ),
    trackProducers = cms.VInputTag(
        "mixedTripletStepTracks" )
)

