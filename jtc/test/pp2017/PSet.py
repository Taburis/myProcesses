
import FWCore.ParameterSet.Config as cms

process = cms.Process("Dummy")
process.source = cms.Source("PoolSource",
		fileNames = cms.untracked.vstring()
		)
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

print(process.source.fileNames)
print("this is the dummy pset file!");
