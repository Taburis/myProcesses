 
import FWCore.ParameterSet.Config as cms

import select_initialStep_cfi

lowPtTripletStepCutSel = select_initialStep_cfi.initialStepCutSel.clone(
    src = cms.InputTag("lowPtTripletStepTracks"),
)

lowPtTripletStepCutBaseTracks = select_initialStep_cfi.initialStepCutBaseTracks.clone(
    inputClassifiers = cms.vstring(
        'lowPtTripletStepCutSel',
    ),
    trackProducers = cms.VInputTag(
        "lowPtTripletStepTracks" )
)
