
#define event_content_skim
#include "cfg_nominal_c5shift_jet60.h"
//#include "cfg_nominal.h"
//#include "cfg_nominal_c5shift.h"
//#include "myProcesses/HIN-20-003/config/cfg.h"
//#include "myProcesses/jtc/plugin/producerBJTC.h"
#include "myProcesses/jtc/plugin/producerBTagging.h"
#include "myProcesses/jtc/plugin/producerEvtQA.h"
#include "myProcesses/jtc/plugin/producerJetQA.h"

using namespace config_AN20029_c5shift;
//using namespace config_AN20029;

void scan(TString input = "", TString output="test.root"){

        //if(input == "") input = "/eos/cms/store/group/phys_heavyions/wangx/HI2018Data/HIHardProbes_HIRun2018A-04Apr2019-v1_CSVTagVars/HIHardProbes/HIHardProbes_HIRun2018A-04Apr2019-v1-CSVTagVars_jet80or100_goldenJSON/201122_193133/0000/skim_100.root";
        //if(input == "") input = "/eos/cms/store/group/phys_heavyions/wangx/HI2018Data/HIHardProbes_HIRun2018A-04Apr2019-v1_CSVTagVars/HIHardProbes/HIHardProbes_HIRun2018A-04Apr2019-v1-CSVTagVars_jet80or100_goldenJSON/201122_193133/0000/skim_100.root";
        if(input == "") input = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/HI2018Data/HIHardProbes_HIRun2018A-04Apr2019-v1_CSVTagVars/HIHardProbesLower/HIHardProbesLower_HIRun2018A-04Apr2019-v1-CSVTagVars_jet80or100_goldenJSON/210202_201810/0000/skim_185.root";
        //if(input == "") input = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/HI2018_HiForestSkim/Bjet_pThat-80_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_bjetSample_jetPt50_privateSample/210503_194556/0000/skim_104.root";
        //if(input == "") input = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/HI2018_HiForestSkim/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_DjetSample_jetPt50_stableOnly/201014_183424/0000/skim_104.root";

	using pset = pset_nominalHI_skim;
	using src  = selections;
	using weight  = weight_data_nominal;
	//using weight  = weight_data_nominal;
	//using weight  = weight_Hydjet_gspWeighted;
	//using weight  = weight_Hydjet_nominal;
	
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 0;
	cfg.ps->isHI = 1;
	cfg.weight->mergeTrig = 1;
//	cfg.src->jeutool.setJERSmear(0.08, 1.15);

	auto f = TFile::Open(input);
	auto lf = new liteFrame<eventMap, config>("test", cfg, f);
//	lf->nevt = 100;
	lf->nevt = -1;
	lf->output = output;
	//auto btag = new producerBTagging<eventMap, config>("bTagging");
	//lf->addProducer(btag);
	auto qa = new producerJetQA<eventMap, config>("jetQA");
	qa->jeccorr.Initialize("../residualJEC/PhiCorrectionGen_AK4PF.txt");
	lf->addProducer(qa);
	qa->addJetSet("inclusive", jetType::inclJet);
	qa->addJetSet("tagged", jetType::taggedJet);
	qa->addJetSet("bjet", jetType::trueBJet);
//	qa->addJetSet("inclJet", jetType::inclJet);
	auto evtInfo = new producerEvtQA<eventMap, config>("evtQA");
//	evtInfo->makeMiniEvtTree=1;
	lf->addProducer(evtInfo);
	lf->run();
}