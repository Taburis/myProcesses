
#include "jtcAnalyzer.h"

#include "TF1.h"
//#include "eventMap_hiForest.h"
float constantf ( float f){
	return 1;
}


float evtf ( eventMap *em){
	return 1;
}

namespace pp2017MC{
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
}

void skimConfig(){
	auto f = TFile::Open("HiForestAOD_1.root");
	auto f2 = TFile::Open("HiForestAOD_1.root");
	
	const int nPt = 6, nCent =1 ;
	float ptbins[nPt+1] = {1, 2, 3, 4, 8, 12, 400};
	float centbins[nCent+1] = {0, 60};
	TString ptLabel[nPt+1] = {"Trk1", "Trk2", "Trk3", "Trk4", "Trk8", "trk12", "Trk400"};
	TString centLabel[nCent+1] = {"Cent0", "Cent30"};

	pp2017MC::initConfig();
	eventMap *em = new eventMap(f);
	em->loadTrack();
	em->loadGenParticle();
	em->loadJet("ak4PFJetAnalyzer");

	
	eventMap *mixem = new eventMap(f2);
	mixem->loadTrack();
	mixem->loadGenParticle();
	auto jtc = new jtcAnalyzer(em);
	jtc->mixem = mixem;
	jtc->domixing = 1;
	jtc->ispp = 1;
	jtc->nevt = -1;
	jtc->nPt = 6;
	jtc->nCent = 1;
	jtc->ptbins = ptbins; 
	jtc->centbins = centbins; 
	jtc->ptLabel = ptLabel; 
	jtc->centLabel = centLabel; 
	jtc->jetPtWeight = pp2017MC::inclJetPtWeight;
	jtc->evtWeight = pp2017MC::inclJetVzWeight;
	jtc->loop();
}
