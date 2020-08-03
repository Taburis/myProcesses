import FWCore.ParameterSet.Config as cms

from Configuration.Generator.HerwigppDefaults_cfi import *
from Configuration.Generator.HerwigppUE_EE_5C_cfi import *


generator = cms.EDFilter("ThePEGGeneratorFilter",
	herwigDefaultsBlock,
	herwigppUESettingsBlock,
	crossSection = cms.untracked.double(1),
	#crossSection = cms.untracked.double(1.712627e+05),
	filterEfficiency = cms.untracked.double(1),

	configFiles = cms.vstring(),
	parameterSets = cms.vstring(
		'herwigppUE_EE_5C_5020GeV',
		'productionParameters',
		'basicSetup',
		'setParticlesStableForDetector',
	),
	productionParameters = cms.vstring(
		'cd /Herwig/MatrixElements/',
		'insert SimpleQCD:MatrixElements[0] MEQCD2to2',

		'cd /',
		'set /Herwig/Cuts/JetKtCut:MinKT 80  *GeV',
		'set /Herwig/Cuts/JetKtCut:MaxKT 300  *GeV',
		'set /Herwig/Cuts/QCDCuts:MHatMin 0.0*GeV',
		'set /Herwig/Cuts/PhotonKtCut:MinKT 0.0*GeV',
		'set /Herwig/Cuts/PhotonKtCut:MinEta -5',
		'set /Herwig/Cuts/PhotonKtCut:MaxEta 5',
	),
)

configurationMetadata = cms.untracked.PSet(
	version = cms.untracked.string('\$Revision$'),
	name = cms.untracked.string('\$Source$'),
	annotation = cms.untracked.string('Sumer2015 sample with HERWIGPP: dijet with pThat = 80 .. 300 GeV, TuneEE5C')
)
