
#include "TROOT.h"
//#include "myProcesses/hiforest/plugin/eventMap_skim.h" // for the hiforest file
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "myProcesses/jtc/plugin/bjtcProducer.h"
#include "myProcesses/jtc/config/config_aa2018_bjet.h"
#include "myProcesses/jtc/plugin/Utility.h"

#include "TF1.h"
float constantf ( float f){
	return 1;
}

float evtf ( eventMap *em){
	return 1;
}


//void makeMixSkim(TString input="root://xrootd.rcac.purdue.edu//store/user/jviinika/PbPb2018_5TeV_MinBiasFiles/HiForestAOD_21.root", TString buffer_out="mixing_buffer.root" ){
void makeMixSkim(TString input="/eos/cms/store/group/phys_heavyions/dhangal/PbPb2018MC_MBHydjetDrum_Jul24/MinBias_Hydjet_Drum5F_2018_5p02TeV/crab_PbPb2018MC_MBHydjetDrum_Jul24/190724_163253/0000/HiForestAOD_102.root", TString buffer_out="mixing_buffer.root" ){
	bool isAA = 1, isMC = 1;

	using namespace AA2018bJet;
	config_init();
	std::vector<std::string> file_name;
	TString infname = input,mixingf = input;

	std::cout<<"loading the file: "<<infname<<std::endl;
	std::cout<<"loading the mixing file: "<<mixingf<<std::endl;
	auto f = TFile::Open(infname);
	auto fmix = TFile::Open(mixingf);

	std::cout<<"files are loaded, loading events..."<<std::endl;
	eventMap *em = new eventMap(f);
	em->isMC = isMC;
	em->AASetup = isAA;
	em->init();
	em->loadTrack();
	if(isMC)em->loadGenParticle();
	//em->loadJet("ak4PFJetAnalyzer");

	eventMap *mixem = new eventMap(fmix);
	mixem->isMC = isMC;
	mixem->AASetup = isAA;
	mixem->init();
	mixem->loadTrack();
	if(isMC)mixem->loadGenParticle();
	mixem->regEventFilter(nEvtFilter, evtFilters);
	auto jtc = new bjtcProducer(em);
	jtc->mixem = mixem;
	if(isAA) jtc->ispp = 0;
	else jtc->ispp = 1;
	//jtc->isMC = 0;
	jtc->isMC = isMC;
	jtc->nevt = -1;
	jtc->nPt = npt;
	jtc->ptbins = ptbins; 
	jtc->ptLabel = ptLabels; 
	jtc->nCent = centHelper.nbins; 
	jtc->centbins = centHelper.hibin; 
	jtc->centLabel = centHelper.makeLabels(); 
	jtc->jetWeight = jetWeight;
	jtc->evtWeight = evtWeight;
	std::cout<<"config loaded, start process:"<<std::endl;
	jtc->vzmin_mix = -15;
	jtc->vzmax_mix = 15;
	jtc->nvz_mix = 60;
	jtc->nsize = 10;
	jtc->hibinmin_mix = hibin_min_mix;
	jtc->hibinmax_mix = hibin_max_mix;
	jtc->ncent_mix = 180;
	//jtc->ncent_mix = nhibin_mix;
	jtc->mixing_buffer_name = buffer_out;
	jtc->setup_mixingTable();
	jtc->scanMixingTable();
	jtc->build_mixing_buff();
	//jtc->loop();
	std::cout<<"Done!"<<std::endl;
}
