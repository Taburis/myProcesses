
#include "TROOT.h"
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "myProcesses/jtc/plugin/jtcFastProducer.h"

#include "TF1.h"
float constantf ( float f){
	return 1;
}

float evtf ( eventMap *em){
	return 1;
}

namespace pp2017MC{
	bool ispp = 1;
	int nevtFilter = 3;
	std::string eventFilters [] = {"pPAprimaryVertexFilter", "HBHENoiseFilterResultRun2Loose", "pBeamScrapingFilter"};
	TF1 fptw("fPtWeightFunction", "pol3", 0, 500);
	TF1 fvzw("fVzWeightFunction", "pol6", -15, 15);
	void initConfig(){
		fptw.SetParameters(0.68323, 0.00481626,-1.1875e-05,1.13663e-08);
		fvzw.SetParameters(0.973805, 0.00339418, 0.000757544, -1.37331e-06, -2.82953e-07, -3.06778e-10, 3.48615e-09);
	}
	float inclJetPtWeight(float pt){
		//return 1;
		return fptw.Eval(pt);
	}
	float inclJetVzWeight(eventMap * em){
		return fvzw.Eval(em->vz);
	}
	const int nPt = 6, nCent =1 ;
	float ptbins[nPt+1] = {1, 2, 3, 4, 8, 12, 400};
	float centbins[nCent+1] = {0, 60};
	TString ptLabel[nPt+1] = {"Trk1", "Trk2", "Trk3", "Trk4", "Trk8", "trk12", "Trk400"};
	TString centLabel[nCent+1] = {"Cent0", "Cent30"};
}


void jtcConfig(bool doCrab = 0, int jobID = 0){

	using namespace pp2017MC;


	std::vector<std::string> file_name;
	TString infname,mixingf = "/store/user/wangx/Pythia8_ppMC2017_bJetOfficialProd_forBJTC_WTAReclustered/QCD_pThat-15_bJet_TuneCP5_5p02TeV_pythia8/0000/HiForestAOD_10.root";
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
	
	initConfig();
	eventMap *em = new eventMap(f);
	em->init();
	em->loadTrack();
	em->loadGenParticle();
	em->loadJet("ak4PFJetAnalyzer");
	em->regEventFilter(nevtFilter, eventFilters);

	eventMap *mixem = new eventMap(fmix);
	mixem->init();
	mixem->loadTrack();
	mixem->loadGenParticle();
	auto jtc = new jtcFastProducer(em);
	jtc->mixem = mixem;
	jtc->domixing = 0;
	jtc->ispp = ispp;
	jtc->nevt = -1;
	jtc->nPt = nPt;
	jtc->nCent = nCent;
	jtc->ptbins = ptbins; 
	jtc->centbins = centbins; 
	jtc->ptLabel = ptLabel; 
	jtc->centLabel = centLabel; 
	jtc->jetPtWeight = inclJetPtWeight;
	jtc->evtWeight = inclJetVzWeight;
	std::cout<<"config loaded, start process:"<<std::endl;
	jtc->loop();
	std::cout<<"Done!"<<std::endl;
}
