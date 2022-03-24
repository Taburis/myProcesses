 
import FWCore.ParameterSet.Config as cms
import select_detachedQuadStep_cfi

pixelPairStepCutSel = select_detachedQuadStep_cfi.detachedQuadStepCutSel.clone(
    src = cms.InputTag("pixelPairStepTracks"),
)

pixelPairStepCutBaseTracks = select_detachedQuadStep_cfi.detachedQuadStepCutBaseTracks.clone(
    inputClassifiers = cms.vstring(
		'pixelPairStepCutSel',
    ),
    trackProducers = cms.VInputTag(
        "pixelPairStepTracks" )
)

