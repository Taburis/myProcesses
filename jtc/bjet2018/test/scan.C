
#define event_content_forest
#include "cfg.h"
#include "myProcesses/jtc/plugin/producerBJTC.h"
#include "myProcesses/jtc/plugin/producerBTagging.h"
#include "myProcesses/jtc/plugin/producerEvtQA.h"
#include "myProcesses/jtc/plugin/producerJetQA.h"
//#include "myProcesses/jtc/plugin/producerJetQA.h"

using namespace config_AN20029;

void scan(TString input = "", TString output="test.root"){

        if(input == "") input = "/eos/cms/store/group/phys_heavyions/wangx/ppRef2017/HISingleMuon_HIRun2017G-v1-MuonInfo_HLTMu5/SingleMuon/HISingleMuon_HIRun2017G-v1-MuonInfo_hiForest/201006_204101/0000/HiForestAOD_1.root";

	using pset = config_AN20029::pset_nominalHI_skim;
	using src  = config_AN20029::selections;
	using weight  = config_AN20029::weight_Hydjet_c5shift;
	
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 0;
	cfg.ps->isHI = 1;

	auto f = TFile::Open(input);
	auto lf = new liteFrame<eventMap, config>("test", cfg, f);
	lf->output = output;
//	auto jp = new producerBJTC<eventMap, config>("jtc");
//	jp->domixing=0;
	//lf->addProducer(jp);
	auto qa = new producerJetQA<eventMap, config>("jetQA");
	lf->addProducer(qa);
	qa->addJetSet("inclJet", jetType::inclJet);
	auto evtInfo = new producerEvtQA<eventMap, config>("evtQA");
//	auto jetid = new producerJetID<eventMap, config>("jetID");
	lf->addProducer(evtInfo);
//	lf->addProducer(jetid);
	lf->run();
}
