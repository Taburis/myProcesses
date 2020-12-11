
#define event_content_skim

#include "myProcesses/HIN-20-003/config/cfg.h"
#include "myProcesses/jtc/plugin/analyzerEvtQA.h"
#include "myProcesses/jtc/plugin/analyzerJetQA.h"

using namespace config_AN20029;

void analysis(){

	using pset = pset_nominalHI_skim;
	using src  = selections;
	using weight  = weight_data_nominal;

        using config = configBase<pset, src, weight>;
        config cfg;

	TString dataf1   = "output_jetQA.root";
	TString dataf2   = "output_jetQA_smeared.root";
	//TString mcfstr   = "result.root";
	auto f1 = TFile::Open(dataf1);
	auto f2 = TFile::Open(dataf2);
	auto an = new analyzerJetQA<config>(cfg);
	//an->loadSet("bJet", f1, "jet80 or 100");
	//an->loadSet("bJet", f2, "jet80, 100 merged");
	an->loadSet("inclusive", f1, "jet80 or 100");
	an->loadSet("inclusive", f2, "jet80, 100 merged");
	//an->output="./";
	an->output="/eos/user/w/wangx/www/AN20-029/triggerMerge";
	an->analyze();

}
