
#include "myProcesses/jtc/plugin/scanPlugin_jetReco.h"
#include "myProcesses/jtc/plugin/scanPlugin_evtInfo.h"
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#include "myProcesses/jtc/config/cfg_AN_20_029_Base.h"

void scan(TString input = "", TString output="test.root"){

	if(input == "") input = "root://xrootd.rcac.purdue.edu//store/user/wangx/PH2018_JetSamples/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_purdue_run1/200325_155632/0000/skim_104.root";
	auto f = TFile::Open(input);

	using pset_load  = configuration::pset_nominal;
	using cuts_load  = configuration::cuts_nominal<eventMap>;
	using weight_load= configuration::weight_pthat<eventMap>;

	using config = configuration::configBase<pset_load, cuts_load, weight_load>;
	config c0;
	auto ss = new simpleScanner<config, eventMap>("test", c0, f);
	auto sp_evtInfo = new scanPlugin_evtInfo<config, eventMap>();
	auto sp_jetreco = new scanPlugin_jetReco<config, eventMap>();
	ss->output = output;
	ss->addPlugin(sp_evtInfo);
	ss->addPlugin(sp_jetreco);
	ss->nevt= -1;
	ss->run();
}
