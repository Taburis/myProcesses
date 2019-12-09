#ifndef FORESTSKIMER_H
#define FORESTSKIMER_H

// system include files
#include <memory>
#include <string>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"



class forestSkimer : public edm::EDAnalyzer {
	struct jetset{
		//jet info (output tree)
		std::vector<float> jeteta, jetphi, jetpt, refpt;
		std::vector<float> discr_csvV2;
		std::vector<float> jet_wta_eta, jet_wta_phi;
		std::vector<int> trackMax;

		//gen jet info
		std::vector<float> genjetpt, genjeteta, genjetphi, genjet_wta_eta, genjet_wta_phi;
	};
	public:
	explicit forestSkimer(const edm::ParameterSet&);
	~forestSkimer();
	virtual void analyze(const edm::Event&, const edm::EventSetup&);
	virtual void endJob();
	void initEventMap();
	bool check_filter();
	void buildOuttree();
	void loadJets(jetset &jet);
	void clearTrk();
	void clearJetset(jetset &jet);
	bool recoJetCut(eventMap *em, int j);
	bool genJetCut(eventMap *em, int j);
	bool genParticleCut(eventMap *em, int j);
	bool trkCut(eventMap*em, int j);
	void loadTrkCuts(edm::ParameterSet &ps);

	eventMap *em;
	private:

	TTree *otree;
	TFile *of;//, *infile;
	edm::Service<TFileService> sf;

	//0. Event Variables (Variable names are same for input and output trees)
	UInt_t          run;
	ULong64_t       evt;
	UInt_t          lumi;

	Float_t vx, vy, vz;
	Int_t hiBin;

	bool isMC = 0;


	bool doJets =0;
	bool ispp = 0;
	jetset jet0;
	std::string _jetname;
	std::vector<std::string> filters;
	int nevtFilter;

	float weight = 1;
	// track part
	bool doTrk = 0;
	std::vector<float> trkpt, trketa, trkphi, trkchi2, trkpterr;
	std::vector<int> highPurity, trkndof, trknlayer, trknhit;
	//gen particle 
	std::vector<float> gpptp, gpetap, gpphip, gppdgIDp;
	std::vector<int> gpchgp;

	//trk cut variables:
	float normChi2, ptmin, hitmin, ptSig, etamax;
};

void forestSkimer::loadJets(jetset &jet){
	otree->Branch("jteta",  &(jet.jeteta));
	otree->Branch("jtphi",  &(jet.jetphi));
	otree->Branch("jtpt" ,  &(jet.jetpt ));
	otree->Branch("WTAeta", &(jet.jet_wta_eta));
	otree->Branch("WTAphi", &(jet.jet_wta_phi));

	otree->Branch("trackMax", &(jet.trackMax));
	otree->Branch("discr_csvV2", &(jet.discr_csvV2));
	if(isMC){
		otree->Branch("refpt",  &(jet.refpt ));
		otree->Branch("genjteta",  &(jet.genjeteta));
		otree->Branch("genjtphi",  &(jet.genjetphi));
		otree->Branch("genjtpt" ,  &(jet.genjetpt));
		otree->Branch("WTAeta", &(jet.genjet_wta_eta));
		otree->Branch("WTAphi", &(jet.genjet_wta_phi));
	}
}
void forestSkimer::clearJetset(jetset &jet){
	jet.jetpt.clear();
	jet.jeteta.clear();
	jet.jetphi.clear();
	jet.jet_wta_eta.clear();
	jet.jet_wta_phi.clear();
	jet.trackMax.clear();
	jet.discr_csvV2.clear();
	if(isMC){
		jet.refpt.clear();
		jet.genjeteta.clear();
		jet.genjetphi.clear();
		jet.genjet_wta_eta.clear();
		jet.genjet_wta_phi.clear();
	}
}

void forestSkimer::buildOuttree(){

	otree->Branch("vz",&(em->vz));
	otree->Branch("hiBin",&(em->hiBin));
	if(doTrk){
		otree->Branch("trkPt", &trkpt);
		otree->Branch("trkEta",&trketa);
		otree->Branch("trkPhi",&trkphi);
		otree->Branch("highPurity",&highPurity);
		otree->Branch("trkNdof",&trkndof);
		otree->Branch("trkChi2",&trkchi2);
		otree->Branch("trkNlayer",&trknlayer);
		otree->Branch("trkNHit",&trknhit);
		otree->Branch("trkPtError",&trkpterr);
		if(isMC){
			otree->Branch("weight",&(em->weight));
			//for gen particles
			otree->SetBranchAddress("pt",  &gpptp);
			otree->SetBranchAddress("eta", &gpetap);
			otree->SetBranchAddress("phi", &gpphip);
			otree->SetBranchAddress("chg", &gpchgp);
			otree->SetBranchAddress("pdg", &gppdgIDp);
		}
	}

}

void forestSkimer::clearTrk(){
	trkpt.clear();
	trketa.clear();
	trkphi.clear();
	trkndof.clear();
	trknlayer.clear();
	trknhit.clear();
	trkchi2.clear();
	trkpterr.clear();
	highPurity.clear();
	gpptp.clear();
	gpetap.clear();
	gpphip.clear();
	gppdgIDp.clear();
	gpchgp.clear();
}


#endif
