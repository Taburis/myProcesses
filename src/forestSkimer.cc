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
//std::cout<<"building inTree"<<std::endl;

  //Reading branches from the input tree 
  
  //0. Event information
  /*
  eventtree->SetBranchAddress("run",&run);
  eventtree->SetBranchAddress("evt",&evt);
  eventtree->SetBranchAddress("lumi",&lumi);
  eventtree->SetBranchAddress("vx",&vx);
  eventtree->SetBranchAddress("vy",&vy);
  eventtree->SetBranchAddress("vz",&vz);
  eventtree->SetBranchAddress("hiBin",&hiBin);
  */


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

    
  otree->Branch("run",&run);
  otree->Branch("evt",&evt);
  otree->Branch("lumi",&lumi);
  
  otree->Branch("vx",&vx);
  otree->Branch("vy",&vy);
  otree->Branch("vz",&vz);
  otree->Branch("hiBin",&hiBin);
  
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
  
  //TFile *infile = TFile::Open("hiForestAOD.root");
  //itree =(TTree*) infile->Get("akPu4PFJetAnalyzer/t");

  cout<<" opening input forst file "<<infile->GetName()<<endl;
  //take this to config file ultimately
  const bool is_PbPb = 1;
  const Int_t radius =4;

  if(is_PbPb){
    itree = (TTree*)infile->Get(Form("akFlowPuCs%dPFJetAnalyzer/t",radius));   // CS Jets
  }else{
    itree = (TTree*)infile->Get(Form("ak%dPFJetAnalyzer/t",radius));
  }
  
  if(!itree){cout << "PF Jet Tree not found!! Exiting..." << endl; exit(1); }

  mutree =(TTree*) infile->Get("ggHiNtuplizerGED/EventTree");
  //itree->AddFriend(mutree);
  buildIntree();
  
  of = TFile::Open("skim.root", "recreate");
  otree = new TTree("mixing_tree", "");
  buildOuttree();
  long nevt = itree->GetEntriesFast();
//  std::cout<<itree->GetName()<<std::endl;
//  std::cout<<nevt<<std::endl;
  for(int ievt = 0; ievt < nevt; ievt++){
    if(ievt% 100) std::cout<<"Processing event "<<ievt<<"...."<<std::endl;
    itree->GetEntry(ievt);
    mutree->GetEntry(ievt);
    //eventtree->GetEntry(ievt);
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
    //infile->Close();
  }
  of->Write();
  of->Close();
}
DEFINE_FWK_MODULE(forestSkimer);
