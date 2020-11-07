
#define event_content_skim
#include "myProcesses/jtc/bjet2018/config/cfg.h"
#include "myProcesses/jtc/plugin/producerBJTC.h"
#include "myProcesses/jtc/plugin/producerBTagging.h"
#include "myProcesses/jtc/plugin/producerEvtQA.h"
//#include "myProcesses/jtc/plugin/producerJetQA.h"

using namespace config_AN20029;

void scan(TString input = "", TString output="test.root"){

        if(input == "") input = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/HI2018_HiForestSkim/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_DjetSample_jetPt50_stableOnly/201014_183424/0000/skim_102.root";
       // if(input == "") input = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/HI2018_skim/HIHardProbes_HIRun2018A-04Apr2019-v1_CSVTagVars/HIHardProbes/HIHardProbes_HIRun2018A-04Apr2019-v1-CSVTagVars_jet80or100/201014_173945/0002/skim_2574.root";

	using pset = config_AN20029::pset_nominalHI_skim;
	using src  = config_AN20029::selections;
	using weight  = config_AN20029::weight_Hydjet_gspWeighted;
	//using weight  = config_AN20029::weight_Hydjet_nominal;
	
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 1;
	cfg.ps->isHI = 1;

	auto f = TFile::Open(input);
	auto lf = new liteFrame<eventMap, config>("test", cfg, f);
	lf->output = output;
	auto btag = new producerBTagging<eventMap, config>("bTagging");
	lf->addProducer(btag);
//	auto qa = new producerJetQA<eventMap, config>("jetQA");
//	lf->addProducer(qa);
//	qa->addJetSet("inclJet", jetType::inclJet);
//	auto evtInfo = new producerEvtQA<eventMap, config>("evtQA");
//	evtInfo->makeMiniEvtTree=1;
//	lf->addProducer(evtInfo);
	lf->run();
}
