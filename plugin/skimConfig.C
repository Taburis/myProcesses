
#include "jtcAnalyzer.h"
//#include "eventMap_hiForest.h"
float constantf ( float f){
	return 1;
}
void skimConfig(){
	auto f = TFile::Open("/mnt/hadoop/store/user/wangx/Pythia8_ppMC2017_bJetOfficialProd_forBJTC_WTAReclustered/QCD_pThat-15_bJet_TuneCP5_5p02TeV_pythia8/QCD_pThat-15_bJet_TuneCP5_5p02TeV_pythia8/crab_QCD_pThat-15_bJet_TuneCP5_5p02TeV_pythia8_addGenAllParticles/191117_211437/0000/HiForestAOD_1.root");
	
	const int nPt = 6, nCent =1 ;
	float ptbins[nPt+1] = {1, 2, 3, 4, 8, 12, 400};
	float centbins[nCent+1] = {0, 60};
	TString ptLabel[nPt+1] = {"Trk1", "Trk2", "Trk3", "Trk4", "Trk8", "trk12", "Trk400"};
	TString centLabel[nCent+1] = {"Cent0", "Cent30"};
	eventMap *em = new eventMap(f);
	em->loadTrack();
	em->loadGenParticle();
	em->loadJet("ak4PFJetAnalyzer");
	auto jtc = new jtcAnalyzer(em);
	jtc->nevt = -1;
	jtc->nPt = 6;
	jtc->nCent = 1;
	jtc->ptbins = ptbins; 
	jtc->centbins = centbins; 
	jtc->ptLabel = ptLabel; 
	jtc->centLabel = centLabel; 
	jtc->jetPtWeight = constantf;
	jtc->loop();
}
