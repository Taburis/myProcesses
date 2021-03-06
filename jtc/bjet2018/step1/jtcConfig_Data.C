
#include "TROOT.h"
//#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "myProcesses/hiforest/plugin/eventMap_skim.h" // for the hiforest file
#include "myProcesses/jtc/plugin/bjtcProducer.h"
#include "myProcesses/jtc/config/config_aa2018_bjet_skim.h" // 2 cent bin format
#include "myProcesses/jtc/plugin/Utility.h"

#include "TF1.h"

void jtcConfig_Data(bool doCrab = 0, int jobID = 0){

	using namespace AA2018bJet;
	config_init();
	std::vector<std::string> file_name;
	TString infname = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/HI2018_HiForest/HIHardProbes_HIRun2018A-04Apr2019-v1_CSVTagVars/HIHardProbes/HIHardProbes_HIRun2018A-04Apr2019-v1-CSVTagVars/200509_045647/0000/skim_104.root";
	TString mixing_buffer ="/eos/cms/store/group/phys_heavyions/wangx/PH2018_JetSamples/mixingBuffer/mixing_buffer_data_ordered_vz60_c180.root";

	if(doCrab){
		ReadFileList(file_name, Form("job_input_file_list_%d.txt",jobID), true);
		infname = file_name.at(0);
		mixing_buffer ="mixing_buffer.root";
	}

	std::cout<<"loading the file: "<<infname<<std::endl;
	auto f = TFile::Open(infname);

	std::cout<<"files are loaded, loading events..."<<std::endl;
	bool isMC = 0;
	eventMap *em = new eventMap(f);
	em->isMC = isMC;
	em->AASetup = 1;
	em->init();
	em->loadTrack();
	em->loadJet("akFlowPuCs4PFJetAnalyzer");
//	em->regEventFilter(nEvtFilter, evtFilters);

	auto jtc = new bjtcProducer(em);

//systematic option-----------------------
	//jtc->setJEU_Down();
	//jtc->setJEU_Up();
	jtc->doJERSmear=1;
//--------------------------------------
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
	jtc->nvz_mix = 60;
	jtc->ncent_mix = 180;
	jtc->nsize = 80;
	jtc->nPerTrig = 30;
	jtc->hibinmin_mix = hibin_min_mix;
	jtc->hibinmax_mix = hibin_max_mix;
	jtc->setup_mixingTable();
	jtc->load_mixing_buffTree(mixing_buffer);
	jtc->loop();
	std::cout<<"Done!"<<std::endl;
}
