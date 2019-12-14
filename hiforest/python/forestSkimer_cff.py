import FWCore.ParameterSet.Config as cms
from HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi import ggHiNtuplizer

forestSkim = cms.EDAnalyzer(
    "forestSkimer",
    doTrk = cms.bool(True),
    doJets = cms.bool(True),
    isPP = cms.bool(True),
    isMC = cms.bool(True),
    event_filter = cms.vstring('pPAprimaryVertexFilter'),
	
    trkCuts = cms.PSet(
	trkptmin = cms.double(1),
	trkptmax = cms.double(400),
	trketamax= cms.double(2.4),
	trkchi2max=cms.double(0.15),
	trknhitmin=cms.int(11),
	trkptsig = cms.double(0.3),
	doHighpurity=cms.bool(1),
	doCaloMatch=cms.bool(1)
    ),
    recoJetCuts = cms.PSet(
	jetptmin = cms.double(100),
	jetetamax= cms.double(1.6)
    ),
    GPCuts = cms.PSet(
	gpPtmin     = cms.double(1),
	gpEtamax    = cms.double(2.4),
	keepNeutral = cms.bool(False)
    )
)

