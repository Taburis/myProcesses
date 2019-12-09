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
	_jetname = iConfig.getParameter<std::string>("jetset");
	doTrk = iConfig.getParameter<bool>("doTrk");
	ispp = iConfig.getParameter<bool>("isPP");
	isMC = iConfig.getParameter<bool>("isMC");
	filters.resize(filterName.size());
}

void forestSkimer::analyze(const Event& iEvent,	 const EventSetup& iSetup) {}
forestSkimer::~forestSkimer() {}

void forestSkimer::initEventMap(){
	em->init();
	em->loadTrack();
	if(isMC) em->loadGenParticle();
	em->loadJet(_jetname);
	em->regEventFilter(nevtFilter, filters);
}

void forestSkimer::endJob() {

	TFile *infile = &(sf->file());

	em = new eventMap(infile);
	initEventMap();

	cout<<" opening input forst file "<<infile->GetName()<<endl;
	
	//take this to config file ultimately


	of = TFile::Open("skim.root", "recreate");
	otree = new TTree("mixing_tree", "");
	buildOuttree();
	loadJets(jet0);
	long nevt = em->evtTree->GetEntriesFast();
	for(int ievt = 0; ievt < nevt; ievt++){
		if(ievt% 100) std::cout<<"Processing event "<<ievt<<"...."<<std::endl;
		em->getEvent(ievt);
		if(em->checkEventFilter()) continue;

		for(int j1 = 0; j1 < em->nref ; j1++)
		{
			Float_t reco_pt  = em->jetpt[j1];
			Float_t reco_phi = em->jetphi[j1];
			Float_t reco_eta = em->jeteta[j1];

			if( fabs(reco_eta) > jteta_max || reco_pt < jtpt_min) continue;

			jet0.jeteta.emplace_back(reco_eta);
			jet0.jetphi.emplace_back(reco_phi);
			jet0.jetpt .emplace_back(reco_pt);

			jet0.trackMax.emplace_back(trackMax[j1]);
			jet0.discr_csvV2.emplace_back(em->disc_csvV2[j1]);
			if(isMC){
				jet0.refpt.emplace_back(em->refpt[j1]);
			}
		}
		for(int i=0; i<em->ntrk; ++i){
			if(trackCut(em, i)) continue;
			trkpt.emplace_back(em->trkpt[i]);
			trkphi.emplace_back(em->trkphi[i]);
			trketa.emplace_back(em->trketa[i]);
			trkpterr.emplace_back(em->trkpterr[i]);
			trkchi2.emplace_back(em->trkchi2[i]);
			trknhit.emplace_back(em->trknhit[i]);
			trknlayer.emplace_back(em->trknlayer[i]);
			trkndof.emplace_back(em->trkndof[i]);
			highPurity.emplace_back(em->highPurity[i]);
		}
		otree->Fill();
		//clear all the vectors 
		clearJetset(jet0);
		//infile->Close();
	}
	of->Write();
	of->Close();
}
DEFINE_FWK_MODULE(forestSkimer);
