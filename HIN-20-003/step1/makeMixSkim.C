
#define event_content_forest
#include "TROOT.h"
//#include "myProcesses/hiforest/plugin/eventMap_skim.h" // for the hiforest file
#include "myProcesses/HIN-20-003/config/cfg_nominal_hiforest.h"
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "producerBJTC.h"

#include "TF1.h"
float constantf ( float f){
	return 1;
}

float evtf ( eventMap *em){
	return 1;
}

using namespace config_AN20029;

void makeMixSkim(TString input="root://xrootd.rcac.purdue.edu//store/user/jviinika/PbPb2018_5TeV_MinBiasFiles/HiForestAOD_21.root", TString buffer_out="mixing_buffer.root" ){
//void makeMixSkim(TString input="/eos/cms/store/group/phys_heavyions/dhangal/PbPb2018MC_MBHydjetDrum_Jul24/MinBias_Hydjet_Drum5F_2018_5p02TeV/crab_PbPb2018MC_MBHydjetDrum_Jul24/190724_163253/0000/HiForestAOD_102.root", TString buffer_out="mixing_buffer.root" ){

	using pset = pset_nominalHI_skim;
	using src  = selections;
	using weight  = weight_data_nominal;
	using config = configBase<pset, src, weight>;
	config cfg;
	cfg.ps->isMC = 0;
	cfg.ps->isHI = 1;
	cfg.weight->mergeTrig = 1;
	bool doMixing = 1;


	std::vector<std::string> file_name;
	TString infname = input,mixingf = input;

	std::cout<<"loading the file: "<<infname<<std::endl;
	std::cout<<"loading the mixing file: "<<mixingf<<std::endl;
	auto f = TFile::Open(infname);
	auto fmix = TFile::Open(mixingf);

	std::cout<<"files are loaded, loading events..."<<std::endl;
	int nEvtFilter = 5;
	string evtFilters []= {"HBHENoiseFilterResultRun2Loose" , "pprimaryVertexFilter","collisionEventSelectionAODv2", "phfCoincFilter2Th4", "pclusterCompatibilityFilter"};

	int nhibin_mix= 180, nvz_mix = 60;
	float hibin_max_mix=180, hibin_min_mix=0;

	eventMap *mixem = new eventMap(fmix);
	mixem->isMC = 0;
	mixem->isHI = 1;
	mixem->init();
	mixem->regEventFilter(nEvtFilter, evtFilters);
	mixem->loadTrack();
	if(mixem->isMC)mixem->loadGenParticle();
	auto jtc = new producerBJTC<eventMap, config>("jtc");
	jtc->_cfg = &cfg;
	jtc->mixem = mixem;
	//jtc->isMC = 0;
	//jtc->isMC = isMC;
	//jtc->nevt = -1;
	//jtc->nPt = npt;
	//jtc->ptbins = ptbins; 
	//jtc->ptLabel = ptLabels; 
	//jtc->nCent = centHelper.nbins; 
	//jtc->centbins = centHelper.hibin; 
	//jtc->centLabel = centHelper.makeLabels(); 
	//jtc->jetWeight = jetWeight;
	//jtc->evtWeight = evtWeight;
	std::cout<<"config loaded, start process:"<<std::endl;
	jtc->vzmin_mix = -15;
	jtc->vzmax_mix = 15;
	jtc->nvz_mix = 60;
	jtc->nsize = 20;
	jtc->hibinmin_mix = hibin_min_mix;
	jtc->hibinmax_mix = hibin_max_mix+10;
	jtc->ncent_mix = 180;
	//jtc->ncent_mix = nhibin_mix;
	jtc->mixing_buffer_name = buffer_out;
	jtc->setup_mixingTable();
	jtc->scanMixingTable();
	jtc->build_mixing_buff();
	//jtc->loop();
	std::cout<<"Done!"<<std::endl;
}
