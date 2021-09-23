 
import FWCore.ParameterSet.Config as cms

import select_initialStep_cfi

highPtTripletStepCutSel = select_initialStep_cfi.initialStepCutSel.clone(
    src = cms.InputTag("highPtTripletStepTracks"),
)

highPtTripletStepCutBaseTracks = select_initialStep_cfi.initialStepCutBaseTracks.clone(
    inputClassifiers = cms.vstring(
        'highPtTripletStepCutSel',
    ),
    trackProducers = cms.VInputTag(
        "highPtTripletStepTracks" )
)
