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
	doMuon = iConfig.getParameter<bool>("doMuon");
	doJets = iConfig.getParameter<bool>("doJets");
}

void forestSkimer::analyze(const Event& iEvent,	 const EventSetup& iSetup) {}
forestSkimer::~forestSkimer() {}

void forestSkimer::buildIntree(){

  if(doJets) {

    itree->SetBranchAddress("nref",&pf_nref);
    itree->SetBranchAddress("jtpt",t_pf_jtpt);
    itree->SetBranchAddress("jteta",t_pf_jteta);
    itree->SetBranchAddress("jtphi",t_pf_jtphi);
    
    itree->SetBranchAddress("trackMax",t_pf_trackMax);
    itree->SetBranchAddress("discr_csvV1", t_pf_discr_csvV1);
    itree->SetBranchAddress("discr_csvV2", t_pf_discr_csvV2);
   






  }
  
  
  
  

  
  if(doMuon) {

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
	if(doJets){
	  
	  otree->Branch("pf_jteta", &pf_jteta);
	  otree->Branch("pf_jtphi", &pf_jtphi);
	  otree->Branch("pf_jtpt", &pf_jtpt);

	  otree->Branch("pf_trackMax", &pf_trackMax);
	  otree->Branch("pf_discr_csvV1", &pf_discr_csvV1);
	  otree->Branch("pf_discr_csvV2", &pf_discr_csvV2);
	


 
	}








}

void forestSkimer::endJob() {
	
  TFile *infile = &(sf->file());
  //infile = TFile::Open("hiForestAOD.root");
  

  //itree =(TTree*) infile->Get("akPu4PFJetAnalyzer/t");

  //take this to config file ultimately
  const bool is_PbPb = 1;
  const Int_t radius =4;


  if(is_PbPb){
    // be careful here, there are no calojets in skim any more
    //calojets are actually akPu4PFJets
    //inp_tree = (TTree*)  my_file->Get(Form("akPu%dCaloJetAnalyzer/t",radius));
    //itree = (TTree*)  my_file->Get(Form("akPu%dPFJetAnalyzer/t",radius));
    //pf_tree = (TTree*) my_file->Get(Form("akPu%dPFJetAnalyzer/t",radius)); // PU Jets
    itree = (TTree*)infile->Get(Form("akFlowPuCs%dPFJetAnalyzer/t",radius));   // CS Jets
  }else{
    //inp_tree = (TTree*)my_file->Get(Form("ak%dCaloJetAnalyzer/t",radius));
    itree = (TTree*)infile->Get(Form("ak%dPFJetAnalyzer/t",radius));
  }
  
  if(!itree){cout << "PF Jet Tree not found!! Exiting..." << endl; exit(1); }
  //itree =(TTree*) infile->Get("akFlowPuCs4PFJetAnalyzer/t");

  mutree =(TTree*) infile->Get("ggHiNtuplizerGED/EventTree");
  //itree->AddFriend(mutree);
  buildIntree();
  
  
  of = TFile::Open("skim.root", "recreate");
  otree = new TTree("mixing_tree", "");
  buildOuttree();
  long nevt = itree->GetEntries();
  for(int ievt = 0; ievt < nevt; ievt++){
    itree->GetEntry(ievt);
    mutree->GetEntry(ievt);

    

    //need to extract arrays from in put Jet tree and set the vectors


    Double_t pf_jteta_max = 2.4;
    Double_t pf_jtpt_min = 30.0;


    for(int j1 = 0; j1 < pf_nref ; j1++)
      {
		
	Float_t reco_pt = t_pf_jtpt[j1];
	Float_t reco_phi = t_pf_jtphi[j1];
	Float_t reco_eta = t_pf_jteta[j1];


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




  }
  of->Write();
  of->Close();
}
DEFINE_FWK_MODULE(forestSkimer);
