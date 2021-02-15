
#define event_content_skim
#include "myProcesses/HIN-20-003/config/cfg.h"
#include "myProcesses/jtc/plugin/producerBJTC.h"
#include "myProcesses/jtc/plugin/producerBTagging.h"
#include "myProcesses/jtc/plugin/producerEvtQA.h"

using namespace config_AN20029;

void evtScan(TString input = "", TString output="test.root"){

        if(input == "") input = "/eos/cms/store/group/phys_heavyions/wangx/HI2018Data/HIHardProbes_HIRun2018A-04Apr2019-v1_CSVTagVars/HIHardProbes/HIHardProbes_HIRun2018A-04Apr2019-v1-CSVTagVars_jet80or100_goldenJSON/201122_193133/0000/skim_10.root";

	using pset = config_AN20029::pset_nominalHI_skim;
	using src  = config_AN20029::selections;
	using weight  = config_AN20029::weight_Hydjet_nominal;
	
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 0;
	cfg.ps->isHI = 1;

	auto f = TFile::Open(input);
	auto lf = new liteFrame<eventMap, config>("test", cfg, f);
	lf->output = output;
	auto evtInfo = new producerEvtQA<eventMap, config>("evtQA");
	evtInfo->addTriggerCheckList(cfg.ps->ntrigs, cfg.ps->trigger);
	evtInfo->makeMiniEvtTree=1;
	lf->addProducer(evtInfo);
	lf->run();
}
