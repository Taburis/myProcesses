import FWCore.ParameterSet.Config as cms
#from HeavyIonsAnalysis.PhotonAnalysis.ggHiNtuplizer_cfi import ggHiNtuplizer

forestSkim = cms.EDAnalyzer(
    "forestSkimer",
    isLegecy = cms.bool(False),
    doTrk = cms.bool(True),
    doWTA = cms.bool(True),
    jetset = cms.string('akFlowPuCs4PFJetAnalyzer'),
    doJets = cms.bool(True),
    isPP = cms.bool(True),
    isMC = cms.bool(True),
    doBtag = cms.bool(True),
    #event_filter = cms.vstring('pprimaryVertexFilter'),
    event_filter = cms.vstring(''),
    trigger = cms.vstring(''),
    muonInfo = cms.vint32(0,0),
    userFS = cms.bool(True),
    inputFile = cms.string(""),
	
    trkCuts = cms.PSet(
	trkptmin = cms.double(1),
	trkptmax = cms.double(400),
	trketamax= cms.double(2.5),
	trkchi2max=cms.double(0.15),
	trknhitsmin=cms.int32(11),
	trkptsig = cms.double(0.3),
	doHighPurity=cms.bool(True),
	doCaloMatch =cms.bool(True)
    ),
    recoJetCuts = cms.PSet(
	jetptmin = cms.double(80),
	jetetamax= cms.double(2)
    ),
    GPCuts = cms.PSet(
	gpPtMin     = cms.double(1),
	gpEtaMax    = cms.double(2.5),
	keepNeutral = cms.bool(False)
    )
)

