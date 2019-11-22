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


forestSkimer::forestSkimer(const edm::ParameterSet& iConfig) :
	filterName (iConfig.getParameter<std::vector<std::string>> ("event_filter"))
{
	doMuon = iConfig.getParameter<bool>("doMuon");
	doJets = iConfig.getParameter<bool>("doJets");
	doTrk = iConfig.getParameter<bool>("doTrk");
	ispp = iConfig.getParameter<bool>("isPP");
	isMC = iConfig.getParameter<bool>("isMC");
	filters.resize(filterName.size());
}

void forestSkimer::analyze(const Event& iEvent,	 const EventSetup& iSetup) {}
forestSkimer::~forestSkimer() {}

void forestSkimer::initEventMap(){

	for( unsigned i = 0; i< filterName.size(); i++){
		itree->SetBranchAddress((filterName[i]).c_str(), &(filters[i]));
	}

	if(doJets) {
		em->loadJet("ak4PFJetAnalyzer");
	}


	if(doTrk){
		em->loadTrack();
	}

	if(doMuon) {
/* not implement yet
		mutree->SetBranchAddress("muPt", &muonPt);
		mutree->SetBranchAddress("muEta", &muonEta);
		mutree->SetBranchAddress("muCharge",        &muonCharge);
		mutree->SetBranchAddress("muType",       &muonType);
		mutree->SetBranchAddress("muIsGlobal",     &muonIsGlobal);
		mutree->SetBranchAddress("muIsTracker",     &muonIsTracker);
		mutree->SetBranchAddress("muIsPF",     &muonIsPF);
		mutree->SetBranchAddress("muIsSTA",     &muonIsSTA);
		mutree->SetBranchAddress("muD0" , &muonD0);
		mutree->SetBranchAddress("muDz" , &muonDz);
		mutree->SetBranchAddress("muChi2NDF", &muonChi2ndf);
		mutree->SetBranchAddress("muInnerD0", &muonInnerD0);
		mutree->SetBranchAddress("muInnerDz", &muonInnerDz);
		mutree->SetBranchAddress("muInnerD0Err", &muonInnerD0Err);
		mutree->SetBranchAddress("muInnerDzErr", &muonInnerDzErr);
		mutree->SetBranchAddress("muInnerPt", 	&muonInnerPt);
		mutree->SetBranchAddress("muInnerPtErr", &muonInnerPtErr);
		mutree->SetBranchAddress("muInnerEta", &muonInnerEta);
		//mutree->SetBranchAddress("muInnerHP", &muonInnerHP);
		mutree->SetBranchAddress("muIsGood",     &muonIsGood);
		mutree->SetBranchAddress("muTrkLayers",  &muonTrkLayers);
		mutree->SetBranchAddress("muPixelLayers",&muonPixelLayers);
		mutree->SetBranchAddress("muPixelHits",  &muonPixelHits);
		mutree->SetBranchAddress("muMuonHits",   &muonMuHits);
		mutree->SetBranchAddress("muTrkQuality", &muonTrkQuality);
		mutree->SetBranchAddress("muStations", &muonStations);
*/
	}
}

void forestSkimer::buildOuttree(){


//	otree->Branch("run",&run);
//	otree->Branch("evt",&evt);
//	otree->Branch("lumi",&lumi);

//	otree->Branch("vx",&vx);
//	otree->Branch("vy",&vy);
	otree->Branch("vz",&(em->vz));
	otree->Branch("hiBin",&hiBin);

	if(doTrk){
		otree->Branch("trkPt", &trkpt);
		otree->Branch("trkEta",&trketa);
		otree->Branch("trkPhi",&trkphi);
		otree->Branch("highPurity",&highPurity);
		otree->Branch("trkNdof",&trkndof);
		otree->Branch("trkChi2",&trkchi2);
		otree->Branch("trkNlayer",&trknlayer);
		otree->Branch("trkPtError",&trkpterr);
		if(isMC){
		//for gen particles
		}
	}

	if(doMuon){
/*
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
		otree->Branch("muonIsGood",     &muonIsGood);
		otree->Branch("muonTrkLayers",  &muonTrkLayers);
		otree->Branch("muonPixelLayers",&muonPixelLayers);
		otree->Branch("muonPixelHits",  &muonPixelHits);
		otree->Branch("muonMuonHits",   &muonMuHits);
		otree->Branch("muonTrkQuality", &muonTrkQuality);
		otree->Branch("muonStations", &muonStations);
*/
	}
	if(doJets){

		otree->Branch("pf_jteta", &pf_jteta);
		otree->Branch("pf_jtphi", &pf_jtphi);
		otree->Branch("pf_jtpt", &pf_jtpt);
		otree->Branch("pf_wta_eta", &pf_jteta);
		otree->Branch("pf_wta_phi", &pf_jtphi);

		otree->Branch("pf_trackMax", &pf_trackMax);
		otree->Branch("pf_discr_csvV1", &pf_discr_csvV1);
		otree->Branch("pf_discr_csvV2", &pf_discr_csvV2);
	}

}

bool forestSkimer::check_filter(){
	//return 1 if the event needs to be skipped
	for( int f : filters){
		if(!f) return 1;
	}
	return 0;
}

void forestSkimer::endJob() {

	TFile *infile = &(sf->file());

	em = new eventMap(infile);

	cout<<" opening input forst file "<<infile->GetName()<<endl;
	
	//take this to config file ultimately

	//mutree =(TTree*) infile->Get("ggHiNtuplizerGED/EventTree");
	//itree->AddFriend(mutree);

	of = TFile::Open("skim.root", "recreate");
	otree = new TTree("mixing_tree", "");
	buildOuttree();
	long nevt = em->evtTree->GetEntriesFast();
	//  std::cout<<itree->GetName()<<std::endl;
	//  std::cout<<nevt<<std::endl;
	for(int ievt = 0; ievt < nevt; ievt++){
		if(ievt% 100) std::cout<<"Processing event "<<ievt<<"...."<<std::endl;
		em->getEvent(ievt);
		if(doMuon) mutree->GetEntry(ievt);
		if(check_filter()) continue;
		//eventtree->GetEntry(ievt);
		//need to extract arrays from in put Jet tree and set the vectors
		Double_t pf_jteta_max = 2.4;
		Double_t pf_jtpt_min = 30.0;


		for(int j1 = 0; j1 < pf_nref ; j1++)
		{
			Float_t reco_pt = em->jetpt[j1];
			Float_t reco_phi = em->jetphi[j1];
			Float_t reco_eta = em->jeteta[j1];

			if( fabs(reco_eta) > pf_jteta_max || reco_pt < pf_jtpt_min) continue;

			pf_jteta.push_back(reco_eta);
			pf_jtphi.push_back(reco_phi);
			pf_jtpt.push_back(reco_pt);

			pf_trackMax.push_back(t_pf_trackMax[j1]);
			pf_discr_csvV1.push_back(t_pf_discr_csvV1[j1]);
			pf_discr_csvV2.push_back(t_pf_discr_csvV2[j1]);
		}
		otree->Fill();
		//clear all the vectors 
		pf_jteta.clear();
		pf_jtphi.clear();
		pf_jtpt.clear();
		//infile->Close();
	}
	of->Write();
	of->Close();
}
DEFINE_FWK_MODULE(forestSkimer);
