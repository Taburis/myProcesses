
#define event_content_skim
#include "cfg.h"
#include "myProcesses/jtc/plugin/producerBJTC.h"
#include "myProcesses/jtc/plugin/producerBTagging.h"
#include "myProcesses/jtc/plugin/producerEvtQA.h"
//#include "myProcesses/jtc/plugin/producerJetQA.h"

using namespace config_AN20029;

void scan(TString input = "", TString output="test.root"){

        if(input == "") input = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/PH2018_bJetSample/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_purdue_run2/200915_211050/0000/skim_101.root";

	using pset = config_AN20029::pset_nominalHI_skim;
	using src  = config_AN20029::selections;
	using weight  = config_AN20029::weight_Hydjet_c5shift;
	
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 1;

	auto f = TFile::Open(input);
	auto lf = new liteFrame<eventMap, config>("test", cfg, f);
	lf->output = output;
//	auto jp = new producerBJTC<eventMap, config>("jtc");
//	jp->domixing=0;
	//lf->addProducer(jp);
	//auto qa = new producerJetQA<eventMap, config>("jetQA");
	//lf->addProducer(qa);
	//qa->addJetSet("inclJet", jetType::inclJet);
	auto evtInfo = new producerEvtQA<eventMap, config>("evtQA");
	auto tagger = new producerBTagging<eventMap, config>("bTagging");
	lf->addProducer(tagger);
	lf->addProducer(evtInfo);
	lf->run();
}
