#include "myProcesses/hiforest/interface/forestSkimer.h"
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
	filters (iConfig.getParameter<std::vector<std::string>> ("event_filter"))
{
	_jetname = iConfig.getParameter<std::string>("jetset");
	doTrk = iConfig.getParameter<bool>("doTrk");
	ispp = iConfig.getParameter<bool>("isPP");
	isMC = iConfig.getParameter<bool>("isMC");
	
	ParameterSet trkPSet = iConfig.getParameter<edm::ParameterSet>("trkCut");
	
}


void forestSkimer::analyze(const Event& iEvent,	 const EventSetup& iSetup) {}

forestSkimer::~forestSkimer() {}

bool forestSkimer::recoJetCut(eventMap *em, int j){
	return 0;
}

void forestSkimer::initEventMap(){
	em->init();
	em->loadTrack();
	if(isMC) em->loadGenParticle();
	em->loadJet(_jetname.c_str());
	em->regEventFilter(filters);
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

		for(int j1 = 0; j1 < em->nJet() ; j1++)
		{
			if(recoJetCut(em,j1)) continue;

			jet0.jeteta.emplace_back(em->jeteta[j1]);
			jet0.jetphi.emplace_back(em->jetphi[j1]);
			jet0.jetpt .emplace_back(em->jetpt [j1]);

			jet0.trackMax.emplace_back(em->jetTrkMax[j1]);
			jet0.discr_csvV2.emplace_back(em->disc_csvV2[j1]);
			if(isMC){
				jet0.refpt.emplace_back(em->ref_jetpt[j1]);
			}
		}
		for(int i=0; i<em->nTrk(); ++i){
			if(trkCut(em, i)) continue;
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
		if(isMC){
			for(int j=0; j< em->nGenJet(); j++){
				if(genJetCut(em,j)) continue;
				jet0.genjeteta.emplace_back(em->genjetpt[j]);
				jet0.genjetphi.emplace_back(em->genjetphi[j]);
				jet0.genjeteta.emplace_back(em->genjeteta[j]);
				jet0.genjet_wta_eta.emplace_back(em->genjet_wta_eta[j]);
				jet0.genjet_wta_phi.emplace_back(em->genjet_wta_phi[j]);
			}
			for(int j=0; j< em->nGP(); j++){
				if(genParticleCut(em, j)) continue;
				gpptp.emplace_back(em->gppt(j));
				gpetap.emplace_back(em->gppt(j));
				gpphip.emplace_back(em->gppt(j));
				gpchgp.emplace_back(em->gpchg(j));
				gppdgIDp.emplace_back(em->gppdgID(j));
			}
		}
		otree->Fill();
		//clear all the vectors 
		clearJetset(jet0);
		clearTrk();
		//infile->Close();
	}
	of->Write();
	of->Close();
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(forestSkimer);
