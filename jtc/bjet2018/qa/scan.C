
#include "cfg_scan.C"

void scan(TString input = "", TString output="test.root"){

	if(input == "") input = "root://xrootd.rcac.purdue.edu//store/user/wangx/PH2018_JetSamples/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_purdue_run1/200325_155632/0000/skim_104.root";
	auto f = TFile::Open(input);
	auto ss = new simpleScanner<config, eventMap>("test", c0, f);
	auto sp_evtInfo = new scanPlugin_evtInfo<config, eventMap>();
	auto sp_jetreco = new scanPlugin_jetReco<config, eventMap>();
	ss->output = output;
	ss->addPlugin(sp_evtInfo);
	ss->addPlugin(sp_jetreco);
	ss->nevt= -1;
	ss->run();
}
