

import FWCore.ParameterSet.Config as cms

forestSkim = cms.EDAnalyzer(
    	"forestSkimer",
	doMuon = cms.untracked.bool(True)
)
