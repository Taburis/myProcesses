
#define event_content_skim

#include "myProcesses/HIN-20-003/config/cfg.h"
#include "myProcesses/jtc/plugin/analyzerEvtQA.h"
#include "myProcesses/jtc/plugin/analyzerJetQA.h"
#include "myProcesses/jtc/plugin/analyzerBTagging.h"

using namespace config_AN20029;

void analysis(){

	using pset = pset_nominalHI_skim;
	using src  = selections;
	using weight  = weight_data_nominal;

        using config = configBase<pset, src, weight>;
        config cfg;

	cfg.ps->isMC = 1;
	cfg.ps->isHI = 1;

	TString datastr   = "output_btagger_data.root";
	TString mcstr   = "output_btagger_MC.root";
	//TString dataf2   = "output_jetQA_smeared.root";
	//TString mcfstr   = "result_mc_jetQA.root";
	//auto f1 = TFile::Open(dataf1);
	//auto f2 = TFile::Open(dataf2);
	//auto fmc = TFile::Open(mcfstr);
	//auto an = new analyzerJetQA<config>(cfg);
	//an->loadSet("bJet", f1, "jet80 or 100");
	//an->loadSet("bJet", f2, "jet80, 100 merged");
	//an->pinPoint = 200;
	//an->loadSet("inclusive", f2, "jet80, 100 merged");
	//an->loadSet("inclusive", f1, "jet80 or 100");
	//an->loadSet("inclusive", fmc, "inclusive jet");
	//an->loadSet("tagged", fmc, "tagged jet");
	//an->doJEC = 1;
	auto an = new analyzerBTagging<config>(cfg);
	an->loadHist(datastr, 0);
	an->loadHist(mcstr, 1);
cout<<"done"<<endl;
	//an->output="./";
	an->output="/eos/user/w/wangx/www/AN20-029";
	an->analyze();

}
