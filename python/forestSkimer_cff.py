import FWCore.ParameterSet.Config as cms
from HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi import ggHiNtuplizer

forestSkim = cms.EDAnalyzer(
    "forestSkimer",
    doMuon = cms.bool(True),
    doJets = cms.bool(True)
)

ggHiNtuplizer.doMuons = forestSkim.doMuon
