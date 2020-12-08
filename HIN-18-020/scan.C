
#define event_content_forest
#include "cfg.h"
#include "myProcesses/jtc/plugin/producerBJTC.h"
#include "myProcesses/jtc/plugin/producerBTagging.h"
#include "myProcesses/jtc/plugin/producerEvtQA.h"
#include "myProcesses/jtc/plugin/producerJetQA.h"

using namespace config_HIN18020;

void scan(TString input = "", TString output="test.root"){

        if(input == "") input = "HiForestAOD_10.root";

	using pset = pset_nominal_forest;
	using src  = selections;
	using weight  = weight_dummy;
	
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 1;
	cfg.ps->isHI = 0;

	auto f = TFile::Open(input);
	auto lf = new liteFrame<eventMap, config>("test", cfg, f);
//	lf->nevt = 100;
	lf->output = output;
	auto qa = new producerJetQA<eventMap, config>("jetQA");
	lf->addProducer(qa);
	qa->addJetSet("inclusive", jetType::inclJet);
//	qa->addJetSet("inclJet", jetType::inclJet);
//	auto evtInfo = new producerEvtQA<eventMap, config>("evtQA");
//	evtInfo->makeMiniEvtTree=1;
//	lf->addProducer(evtInfo);
	lf->run();
}
