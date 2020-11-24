#include "myProcesses/jtc/plugin/scanPlugin_evtInfo.h"
#include "myProcesses/jtc/plugin/scanPlugin_jetReco.h"
#include "myProcesses/jtc/plugin/scanPlugin_jetQA.h"
//#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#include "cfg_scan.C"

void scan(TString input = "", TString output="test.root"){

	if(input == "") input = "root://cmsxrootd.fnal.gov//store/user/kumarv/UICWork/MuonJetAna/Forest/Data/PP2017Data_HLT_HIL3Mu5_AK4PFJet30/SingleMuon/crab_PP2017Data_HLT_HIL3Mu5_AK4PFJet30/190822_212321/0000/HiForestAOD_PP2017Data_Mu5PFJet30Trigger_101.root";

	using pset_load  = configuration::pset_nominalpp;
	using cuts_load  = configuration::cuts_nominal<eventMap>;
	using weight_load= configuration::weight_pythia_c5shift<eventMap>;

	using config = configuration::configBase<pset_load, cuts_load, weight_load>;
	config c0;

	auto f = TFile::Open(input);
	c0.isMC = 0;
	c0.isHI = 0;
	auto ss = new simpleScanner<config, eventMap>("test", c0, f);
	auto sp_evtInfo = new scanPlugin_evtInfo<config, eventMap>();
	//	auto sp_jetreco = new scanPlugin_jetReco<config, eventMap>();
	auto sp_jetQA = new scanPlugin_jetQA<config, eventMap>();

	ss->output = output;
	ss->addPlugin(sp_evtInfo);
	ss->addPlugin(sp_jetQA);
	sp_jetQA->addJetSet("inclJet",jetType::inclJet);
	sp_jetQA->jetSelection = jetSelection;

	ss->nevt= -1;
	ss->run();
}
