
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




void makeMixSkim(TString input="root://xrootd.rcac.purdue.edu//store/user/wangx/PH2018_JetSamples/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_JetClean_Purdue/200202_224121/0000/skim_102.root", TString buffer_out="mixing_buffer.root" ){
//void makeMixSkim(TString input="/eos/cms/store/group/phys_heavyions/dhangal/PbPb2018MC_MBHydjetDrum_Jul24/MinBias_Hydjet_Drum5F_2018_5p02TeV/crab_PbPb2018MC_MBHydjetDrum_Jul24/190724_163253/0000/HiForestAOD_102.root", TString buffer_out="mixing_buffer.root" ){
//void makeMixSkim(bool doCrab = 0, int jobID = 0){

	using namespace AA2018bJet;
	config_init();
	std::vector<std::string> file_name;
	TString infname = input,mixingf = input;

	std::cout<<"loading the file: "<<infname<<std::endl;
	std::cout<<"loading the mixing file: "<<mixingf<<std::endl;
	auto f = TFile::Open(infname);
	auto fmix = TFile::Open(mixingf);

	std::cout<<"files are loaded, loading events..."<<std::endl;
//	eventMap *em ;
	eventMap *em = new eventMap(f);
	em->init();
	em->loadTrack();
	em->loadGenParticle();
	em->loadJet("ak4PFJetAnalyzer");
	em->regEventFilter(nEvtFilter, evtFilters);

	eventMap *mixem = new eventMap(fmix);
	mixem->init();
	mixem->loadTrack();
	mixem->loadGenParticle();
	auto jtc = new bjtcProducer(em);
	jtc->mixem = mixem;
	jtc->ispp = 0;
	jtc->isMC = 1;
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
