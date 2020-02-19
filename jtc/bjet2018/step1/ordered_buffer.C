
#include "TROOT.h"
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "myProcesses/jtc/plugin/bjtcProducer.h"
#include "myProcesses/jtc/plugin/PLOTLIB/multi_pads.h"
#include "myProcesses/jtc/config/config_aa2018_bjet.h"
#include "myProcesses/jtc/plugin/Utility.h"

#include "TF1.h"
float constantf ( float f){
	return 1;
}

float evtf ( eventMap *em){
	return 1;
}

void ordered_buffer(TString input="../data/minbias2018_MC_mixingBuffer_unordered.root"){
//void makeMixSkim(bool doCrab = 0, int jobID = 0){
	//input = "mixing_buffer.root";

	using namespace AA2018bJet;
	config_init();
	std::vector<std::string> file_name;
//	if(doCrab){
//		ReadFileList(file_name, Form("job_input_file_list_%d.txt",jobID), true);
//		infname = eos_prefix+file_name.at(0);
//		mixingf = eos_prefix+mixingf;
//	} else {
//		infname = eos_prefix+mixingf;
//		mixingf = eos_prefix+mixingf;
//	}


//	eventMap *em ;
	auto jtc = new bjtcProducer();
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
	jtc->nvz_mix = 30;
	jtc->nsize = 100;
	jtc->hibinmin_mix = hibin_min_mix;
	jtc->hibinmax_mix = hibin_max_mix;
	jtc->ncent_mix = nhibin_mix;
	jtc->setup_mixingTable();
	jtc->load_mixing_buffTree(input);
	jtc->checkMixingTable(1);

	jtc->dump_mixing_buffer("./mixing_buffer.root");
	
	std::cout<<"Done!"<<std::endl;
}
