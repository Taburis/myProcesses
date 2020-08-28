
#include "TROOT.h"
//#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "myProcesses/hiforest/plugin/eventMap_skim.h" // for the hiforest file
#include "myProcesses/jtc/plugin/bjtcProducer.h"
//#include "myProcesses/jtc/config/config_aa2018_bjet.h"  // 4 cent bin format
//#include "myProcesses/jtc/config/config_aa2018_bjet_skim_4cent_shift.h" // 4 cent bin format
//#include "myProcesses/jtc/config/config_aa2018_bjet_skim.h" // 2 cent bin format
#include "myProcesses/jtc/config/config_aa2018_bjet_skim_shift.h" // 2 cent bin format
#include "myProcesses/jtc/plugin/Utility.h"

#include "TF1.h"

void jtcConfig_MC(bool doCrab = 0, int jobID = 0){

	using namespace AA2018bJet;
	config_init();

	auto weightff = TFile::Open("weight.root");
//	hist_cent_extra = (TH1*) weightff ->Get("centWeight_extra");

	std::vector<std::string> file_name;
//	TString infname = "skim.root";
	TString infname = "root://xrootd.rcac.purdue.edu//store/user/wangx/PH2018_JetSamples/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_purdue_run1/200325_155632/0000/skim_105.root";
	TString mixing_buffer ="/eos/cms/store/group/phys_heavyions/wangx/mixingBuffer/mixing_buffer_MC_ordered_sube_vz60_hi180.root";

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
	em->loadBTagger();
	em->loadGenParticle();
	em->loadJet("ak4PFJetAnalyzer");
	//em->regEventFilter(nEvtFilter-1, evtFilters_skimPatch);

	auto jtc = new bjtcProducer(em);
	jtc->dosube=1;
	jtc->ispp = 0;
	jtc->domixing = 1;
	jtc->isMC = isMC;
	jtc->nevt = -1;
	jtc->jtpt_min = 120; // for spill over jets 
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
	jtc->nvz_mix = nvz_mix;
	jtc->ncent_mix = nhibin_mix;
	jtc->nsize = 80;
	jtc->nPerTrig = 50;
	jtc->hibinmin_mix = hibin_min_mix;
	jtc->hibinmax_mix = hibin_max_mix;
	jtc->setup_mixingTable();
	if(jtc->domixing) jtc->load_mixing_buffTree(mixing_buffer);
	jtc->loop();
	std::cout<<"Done!"<<std::endl;
}

