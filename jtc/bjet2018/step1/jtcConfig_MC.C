
#include "TROOT.h"
#include "myProcesses/hiforest/plugin/eventMap_skim.h" // for the hiforest file
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


void jtcConfig_MC(bool doCrab = 0, int jobID = 0){

	using namespace AA2018bJet;
	config_init();
	std::vector<std::string> file_name;
	//TString infname, mixingf = "skim.root";
	TString infname, mixingf = "root://xrootd.rcac.purdue.edu//store/user/wangx/PH2018_JetSamples/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_JetClean_Purdue/200202_224121/0000/skim_520.root";
	TString eos_prefix = "";
	TString mixing_buffer ="mixing_buffer.root";

	if(doCrab){
		ReadFileList(file_name, Form("job_input_file_list_%d.txt",jobID), true);
		infname = eos_prefix+file_name.at(0);
	} else {
		infname = eos_prefix+mixingf;
	}

	std::cout<<"loading the file: "<<infname<<std::endl;
	auto f = TFile::Open(infname);

	std::cout<<"files are loaded, loading events..."<<std::endl;
	bool isMC = 1;
//	eventMap *em ;
	eventMap *em = new eventMap(f);
	em->isMC = isMC;
	em->AASetup = 1;
	em->init();
	em->loadTrack();
	em->loadGenParticle();
	em->loadJet("ak4PFJetAnalyzer");
	em->regEventFilter(0, nullptr);
	//em->regEventFilter(nEvtFilter, evtFilters);

	auto jtc = new bjtcProducer(em);
	jtc->ispp = 0;
	jtc->domixing = 1;
	jtc->isMC = isMC;
	jtc->nevt = -1;
	jtc->jtpt_max = 110; // for spill over jets 
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
	jtc->nvz_mix = 30;
	jtc->nsize = 5;
	jtc->nPerTrig = 25;
	jtc->hibinmin_mix = hibin_min_mix;
	jtc->hibinmax_mix = hibin_max_mix;
	jtc->ncent_mix = nhibin_mix;
	jtc->setup_mixingTable();
	jtc->load_mixing_buffTree(mixing_buffer);
	jtc->loop();
	std::cout<<"Done!"<<std::endl;
}
