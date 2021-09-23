 
import FWCore.ParameterSet.Config as cms

import select_initialStep_cfi

lowPtQuadStepCutSel = select_initialStep_cfi.initialStepCutSel.clone(
    src = cms.InputTag("lowPtQuadStepTracks"),
)

lowPtQuadStepCutBaseTracks= select_initialStep_cfi.initialStepCutBaseTracks.clone(
    inputClassifiers = cms.vstring(
        'lowPtQuadStepCutSel',
    ),
    trackProducers = cms.VInputTag(
        "lowPtQuadStepTracks" )
)
