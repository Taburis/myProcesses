
#include "plugin/eventMap_hiForest.h" // for the hiforest file
#include "plugin/jtcProducer.h"

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
		fvzw.SetParameters(1.00656,-0.0193651,0.000976851,-1.043e-05,-9.79808e-06,9.07733e-08,1.79165e-08);
	}
	float inclJetPtWeight(float pt){
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


void jtcConfig_for_condor(std::string outputName= "output.root", std::string infname="/mnt/hadoop/store/user/wangx/Pythia8_ppMC2017_bJetOfficialProd_forBJTC_WTAReclustered/QCD_pThat-15_bJet_TuneCP5_5p02TeV_pythia8/0000/HiForestAOD_103.root"){

	using namespace pp2017MC;


	TString mixingf = "/mnt/hadoop/store/user/wangx/Pythia8_ppMC2017_bJetOfficialProd_forBJTC_WTAReclustered/QCD_pThat-15_bJet_TuneCP5_5p02TeV_pythia8/0000/HiForestAOD_103.root";


	std::cout<<"loading the file: "<<infname<<std::endl;
	std::cout<<"loading the mixing file: "<<mixingf<<std::endl;
	auto f = TFile::Open(infname.c_str());
	auto fmix = TFile::Open(mixingf);

	std::cout<<"files are loaded, loading events..."<<std::endl;
	
	initConfig();
	eventMap *em = new eventMap(f);
	em->loadTrack();
	em->loadGenParticle();
	em->loadJet("ak4PFJetAnalyzer");
	em->regEventFilter(nevtFilter, eventFilters);

	eventMap *mixem = new eventMap(fmix);
	mixem->loadTrack();
	mixem->loadGenParticle();
	auto jtc = new jtcProducer(em);
	jtc->outputName = outputName;
	jtc->mixem = mixem;
	jtc->domixing = 1;
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
