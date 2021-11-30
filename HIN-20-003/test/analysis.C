
#define event_content_skim

#include "myProcesses/HIN-20-003/config/cfg_nominal_c5shift.h"
//#include "myProcesses/HIN-20-003/config/cfg.h"
#include "myProcesses/jtc/plugin/analyzerEvtQA.h"
#include "myProcesses/jtc/plugin/analyzerJetQA.h"
#include "myProcesses/jtc/plugin/analyzerBTagging.h"

using namespace config_AN20029_c5shift;

//using namespace config_AN20029;

void analysis(){

	using pset = pset_nominalHI_skim;
	using src  = selections;
	using weight  = weight_data_nominal;

        using config = configBase<pset, src, weight>;
        config cfg;

	cfg.ps->isMC = 0;
	cfg.ps->isHI = 1;

	//TString mcfstr  = "output_mc_jec.root";
	//auto fmc = TFile::Open(mcfstr);
	//TString dataf2   = "output_jetQA_smeared.root";
	//TString datastr   = "btag_scan_data.root";
	//TString mcfstr   = "output_btagger_gsp.root";
	//TString mcfstr   = "output_MC_jetQA.root";
	//TString mcfstr   = "scan_dijetMC_jetQA.root";
	TString mcfstr   = "test_jec2d.root";
	//TString mcfstr   = "jec_private_bjetMC.root";
	auto fmc = TFile::Open(mcfstr);
	//TString mcfstr   = "btag_scan_djet_mc_weighted.root";
	//TString dataf1= "out_trig6080100.root";
	//TString dataf2= "out_trig80100.root";
	//TString dataf1= "trig_study_6080100.root";
	//TString dataf2= "trig_study_80100.root";
	//auto f1 = TFile::Open(dataf1);
	//auto f2 = TFile::Open(dataf2);
	auto an = new analyzerJetQA<config>(cfg);
	//an->loadSet("bJet", f1, "jet80 or 100");
	//an->loadSet("bJet", f2, "jet80, 100 merged");
	//an->pinPoint = 200;
	//an->loadSet("inclusive", f1, "jet60, 80, 100");
	//an->loadSet("inclusive", f2, "jet80, 100");
	an->loadSet("inclusive", fmc, "inclusive jet");
	an->loadSet("tagged", fmc, "tagged jet");
	an->loadSet("bjet", fmc, "b jet");
	an->doJEC = 1;
	//auto an = new analyzerBTagging<config>(cfg);
	//an->loadHist(datastr, 0);
	//an->loadHist(mcfstr, 1);
	an->output="./";
	an->output="/eos/user/w/wangx/www/AN20-029/test/";
	//an->output="/eos/user/w/wangx/www/AN20-029/";
	//an->output="/eos/user/w/wangx/www/AN20-029/TaggerStudy/";
	an->analyze();

}
