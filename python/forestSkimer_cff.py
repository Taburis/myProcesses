import FWCore.ParameterSet.Config as cms
from HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi import ggHiNtuplizer

forestSkim = cms.EDAnalyzer(
    "forestSkimer",
    doMuon = cms.bool(False),
    doJets = cms.bool(False)
)

ggHiNtuplizer.doMuons = forestSkim.doMuon
