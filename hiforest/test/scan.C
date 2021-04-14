
#define event_content_forest
#include "cfg.h"
#include "myProcesses/hiforest/plugin/producerSkimer.h"

using namespace config_skim_pp;

void scan(TString input = "", TString output="test.root"){

        if(input == "") input = "/eos/cms/store/group/phys_heavyions/dhangal/pp2017MCForest_5TeV_AllQCDPhoton_pthat170/QCDPhoton_pThat-170_TuneCP5_5p02TeV_pythia8/crab_pp2017MCForest_5TeV_AllQCDPhoton_pthat170/190703_173407/0000/HiForestAOD_1.root";

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
	auto skimer = new producerSkimer<eventMap, config>("skimer");
	lf->addProducer(skimer);
	lf->run();
}
