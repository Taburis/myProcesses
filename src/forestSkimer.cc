
#include "HeavyIonsAnalysis/myProcesses/interface/forestSkimer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
//#include <Math/DistFunc.h>
#include "TMath.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/EventSetup.h"


using namespace std;
using namespace edm;


forestSkimer::forestSkimer(const edm::ParameterSet& iConfig) {}
void forestSkimer::analyze(const Event& iEvent,	 const EventSetup& iSetup) {}
forestSkimer::~forestSkimer() {}

void forestSkimer::buildIntree(){
	itree->SetBranchAddress("muPt", &muonPt);
}

void forestSkimer::buildOuttree(){
	otree->Branch("muonPt", &muonPt);	
}

void forestSkimer::endJob() {
	TFile *infile = &(sf->file());
	//infile = TFile::Open("hiForestAOD.root");
	itree =(TTree*) infile->Get("akPu4PFJetAnalyzer/t");
	auto mutree =(TTree*) infile->Get("ggHiNtuplizerGED/EventTree");
	itree->AddFriend(mutree);
	buildIntree();


	of = TFile::Open("skim.root", "recreate");
	otree = new TTree("mixing_tree", "");
	buildOuttree();
	long nevt = itree->GetEntries();
	for(int ievt = 0; ievt < nevt; ievt++){
		itree->GetEntry(ievt);
	}
	otree->Fill();
	of->Write();
	of->Close();
}




DEFINE_FWK_MODULE(forestSkimer);
