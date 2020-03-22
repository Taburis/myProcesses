
#include "TROOT.h"
//#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
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
	TString infname = "/afs/cern.ch/user/w/wangx/workSpace/public/bjet2018_SW_10_3_3_patch1/src/myProcesses/jtc/bjet2018/step1/skim.root";
//	TString infname = "root://xrootd.rcac.purdue.edu//store/user/wangx/PH2018_JetSamples/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_JetClean_eos/200221_055552/0000/skim_110.root";
	TString mixing_buffer ="/eos/cms/store/group/phys_heavyions/wangx/PH2018_JetSamples/mixingBuffer/minbias_MC_mixing_buffer.root";

	if(doCrab){
		ReadFileList(file_name, Form("job_input_file_list_%d.txt",jobID), true);
		infname = file_name.at(0);
		mixing_buffer ="mixing_buffer.root";
	}

	std::cout<<"loading the file: "<<infname<<std::endl;
	auto f = TFile::Open(infname);

	std::cout<<"files are loaded, loading events..."<<std::endl;
	bool isMC = 1;
	eventMap *em = new eventMap(f);
	em->isMC = isMC;
	em->AASetup = 1;
	em->init();
	em->loadTrack();
	em->loadGenParticle();
	em->loadJet("ak4PFJetAnalyzer");
	//em->regEventFilter(nEvtFilter-1, evtFilters_skimPatch);

	auto jtc = new bjtcProducer(em);
	jtc->ispp = 0;
	jtc->domixing = 1;
	jtc->isMC = isMC;
	jtc->nevt = -1;
	jtc->jtpt_max = 120; // for spill over jets 
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
	jtc->ncent_mix = 180;
	//jtc->nvz_mix = 30;
	//jtc->ncent_mix = nhibin_mix;
	jtc->nsize = 80;
	jtc->nPerTrig = 20;
	jtc->hibinmin_mix = hibin_min_mix;
	jtc->hibinmax_mix = hibin_max_mix;
	jtc->setup_mixingTable();
	jtc->load_mixing_buffTree(mixing_buffer);
	jtc->loop();
	std::cout<<"Done!"<<std::endl;
}
