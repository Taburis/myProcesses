
#include "TROOT.h"
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "myProcesses/jtc/plugin/bjtcProducer.h"
#include "myProcesses/jtc/plugin/config_aa2018_bjet.h"
#include "myProcesses/jtc/plugin/Utility.h"

#include "TF1.h"
float constantf ( float f){
	return 1;
}

float evtf ( eventMap *em){
	return 1;
}




void makeMixSkim(bool doCrab = 0, int jobID = 0){

	using namespace AA2018bJet;
	config_init();
	std::vector<std::string> file_name;
	TString infname,mixingf = "/store/user/jviinika/PbPb2018_5TeV_MinBiasFiles/HiForestAOD_32.root";
	//TString infname,mixingf = "gsiftp://cms-gridftp.rcac.purdue.edu//store/user/wangx/Pythia8_ppMC2017_bJetOfficialProd_forBJTC_WTAReclustered/QCD_pThat-15_bJet_TuneCP5_5p02TeV_pythia8/0000/HiForestAOD_103.root";
	//std::string EOS_PURDUE_T2_prefix = "root://xrootd.rcac.purdue.edu/"; 
	std::string EOS_PURDUE_T2_prefix = "/mnt/hadoop"; 
	std::string eos_prefix = EOS_PURDUE_T2_prefix;

	if(doCrab){
		ReadFileList(file_name, Form("job_input_file_list_%d.txt",jobID), true);
		infname = eos_prefix+file_name.at(0);
		mixingf = eos_prefix+mixingf;
	} else {
		infname = eos_prefix+mixingf;
		mixingf = eos_prefix+mixingf;
	}

	std::cout<<"loading the file: "<<infname<<std::endl;
	std::cout<<"loading the mixing file: "<<mixingf<<std::endl;
	auto f = TFile::Open(infname);
	auto fmix = TFile::Open(mixingf);

	std::cout<<"files are loaded, loading events..."<<std::endl;
	eventMap *em = new eventMap(f);
	//eventMap *em = new eventMap(infname);
//	em->init();
//	em->loadTrack();
//	em->loadGenParticle();
//	em->loadJet("ak4PFJetAnalyzer");
//	em->regEventFilter(nEvtFilter, evtFilters);

	eventMap *mixem = new eventMap(fmix);
	mixem->init();
	mixem->loadTrack();
	mixem->loadGenParticle();
	auto jtc = new bjtcProducer(em);
	jtc->mixem = mixem;
	jtc->domixing = 1;
	jtc->ispp = 0;
	jtc->isMC = 0;
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
	jtc->nsize = 5;
	jtc->hibinmin_mix = hibin_min_mix;
	jtc->hibinmax_mix = hibin_max_mix;
	jtc->ncent_mix = nhibin_mix;
	jtc->setup_mixingTable();
	jtc->scanMixingTable();
	jtc->build_mixing_buff();
	//jtc->loop();
	std::cout<<"Done!"<<std::endl;
}
