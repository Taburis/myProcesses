
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


forestSkimer::forestSkimer(const edm::ParameterSet& iConfig) {
	doMuon = iConfig.getParameter<bool>("doMuons");
}

void forestSkimer::analyze(const Event& iEvent,	 const EventSetup& iSetup) {}
forestSkimer::~forestSkimer() {}

void forestSkimer::buildIntree(){
	if(doMuon) {
		itree->SetBranchAddress("muPt", &muonPt);
		itree->SetBranchAddress("muEta", &muonEta);
		itree->SetBranchAddress("muCharge",        &muonCharge);
		itree->SetBranchAddress("muType",       &muonType);
		itree->SetBranchAddress("muIsGlobal",     &muonIsGlobal);
		itree->SetBranchAddress("muIsTracker",     &muonIsTracker);
		itree->SetBranchAddress("muIsPF",     &muonIsPF);
		itree->SetBranchAddress("muIsSTA",     &muonIsSTA);
		itree->SetBranchAddress("muD0" , &muonD0);
		itree->SetBranchAddress("muDz" , &muonDz);
		itree->SetBranchAddress("muChi2NDF", &muonChi2ndf);
		itree->SetBranchAddress("muInnerD0", &muonInnerD0);
		itree->SetBranchAddress("muInnerDz", &muonInnerDz);
		itree->SetBranchAddress("muInnerD0Err", &muonInnerD0Err);
		itree->SetBranchAddress("muInnerDzErr", &muonInnerDzErr);
		itree->SetBranchAddress("muInnerPt", 	&muonInnerPt);
		itree->SetBranchAddress("muInnerPtErr", &muonInnerPtErr);
		itree->SetBranchAddress("muInnerEta", &muonInnerEta);
		//	itree->SetBranchAddress("muInnerHP", &muonInnerHP);
		itree->SetBranchAddress("muIsGood",     &muonIsGood);
		itree->SetBranchAddress("muTrkLayers",  &muonTrkLayers);
		itree->SetBranchAddress("muPixelLayers",&muonPixelLayers);
		itree->SetBranchAddress("muPixelHits",  &muonPixelHits);
		itree->SetBranchAddress("muMuonHits",   &muonMuHits);
		itree->SetBranchAddress("muTrkQuality", &muonTrkQuality);
		itree->SetBranchAddress("muStations", &muonStations);
	}

}

void forestSkimer::buildOuttree(){

	if(doMuon){
		otree->Branch("muonPt", &muonPt);	
		otree->Branch("muonEta", &muonEta);
		otree->Branch("muonCharge",        &muonCharge);
		otree->Branch("muonType",       &muonType);
		otree->Branch("muonIsGlobal",     &muonIsGlobal);
		otree->Branch("muonIsTracker",     &muonIsTracker);
		otree->Branch("muonIsPF",     &muonIsPF);
		otree->Branch("muonIsSTA",     &muonIsSTA);
		otree->Branch("muonD0" , &muonD0);
		otree->Branch("muonDz" , &muonDz);
		otree->Branch("muonChi2NDF", &muonChi2ndf);
		otree->Branch("muonInnerD0", &muonInnerD0);
		otree->Branch("muonInnerDz", &muonInnerDz);
		otree->Branch("muonInnerD0Err", &muonInnerD0Err);
		otree->Branch("muonInnerDzErr", &muonInnerDzErr);
		otree->Branch("muonInnerPt", 	&muonInnerPt);
		otree->Branch("muonInnerPtErr", &muonInnerPtErr);
		otree->Branch("muonInnerEta", &muonInnerEta);
		//	otree->Branch("muonInnerHP", &muonInnerHP);
		otree->Branch("muonIsGood",     &muonIsGood);
		otree->Branch("muonTrkLayers",  &muonTrkLayers);
		otree->Branch("muonPixelLayers",&muonPixelLayers);
		otree->Branch("muonPixelHits",  &muonPixelHits);
		otree->Branch("muonMuonHits",   &muonMuHits);
		otree->Branch("muonTrkQuality", &muonTrkQuality);
		otree->Branch("muonStations", &muonStations);
	}

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
		otree->Fill();
	}
	of->Write();
	of->Close();
}




DEFINE_FWK_MODULE(forestSkimer);
