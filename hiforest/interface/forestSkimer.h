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
	static const int jetMax = 999;
	struct jetset{
		int njet=0, ngj=0;
		//jet info (output tree)
		Float_t jetpt[jetMax], jeteta[jetMax], jetphi[jetMax], refpt[jetMax];
		Float_t discr_csvV2[jetMax], pdiscr_csvV2[jetMax], ndiscr_csvV2[jetMax];
		Float_t jet_wta_eta[jetMax], jet_wta_phi[jetMax];
		Int_t trackMax[jetMax];
		Int_t matchedHadronFlavor[jetMax], genMatchIndex[jetMax];

		//gen jet info
		Float_t genjetpt[jetMax], genjeteta[jetMax], genjetphi[jetMax], genjet_wta_eta[jetMax], genjet_wta_phi[jetMax];	
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
	//trk cuts:
	double trkptmin,trkptmax, trketamax, trkptsig, trkchi2max;
	int trknhitmin;
	bool doHighpurity, doCaloMatch;
	//jet cuts;
	double jetptmin, jetetamax;
	//gen particle cuts:
	double genptmin, genetamax;
	bool keepNeutral = 0;

	bool doJets =0;
	bool ispp = 0;
	jetset jet0;
	std::string _jetname;
	std::vector<std::string> filters;
	int nevtFilter;

	float weight = 1;
	float pthat = -1;
	// track part
	bool doTrk = 0;
	int ntrk=0;
	static const int trkMax = 9999;
	Float_t trkpt[trkMax], trketa[trkMax], trkphi[trkMax], trkchi2[trkMax], trkpterr[trkMax];
	Float_t pfEcal[trkMax], pfHcal[trkMax];
	UChar_t  trkndof[trkMax], trknlayer[trkMax], trknhit[trkMax];
	Bool_t highPurity[trkMax];
	//gen particle 
	std::vector<float> gpptp, gpetap, gpphip, gppdgIDp;
	std::vector<int> gpchgp;

	//trk cut variables:
	float normChi2, ptmin, hitmin, ptSig, etamax;
};

void forestSkimer::loadJets(jetset &jet){
	otree->Branch("nref" ,  &jet.njet);
	otree->Branch("jteta",  &(jet.jeteta),"jteta[nref]/F");
	otree->Branch("jtphi",  &(jet.jetphi),"jtphi[nref]/F");
	otree->Branch("jtpt" ,  &(jet.jetpt ),"jtpt[nref]/F");
	otree->Branch("WTAeta", &(jet.jet_wta_eta),"WTAeta[nref]/F");
	otree->Branch("WTAphi", &(jet.jet_wta_phi),"WTAphi[nref]/F");

	otree->Branch("trackMax",&(jet.trackMax),"trkacMax[nref]/I");
	otree->Branch("discr_csvV2", &(jet.discr_csvV2),"discr_csvV2[nref]/F");
	otree->Branch("pdiscr_csvV2", &(jet.pdiscr_csvV2),"pdiscr_csvV2[nref]/F");
	otree->Branch("ndiscr_csvV2", &(jet.ndiscr_csvV2),"ndiscr_csvV2[nref]/F");
	otree->Branch("matchedHadronFlavor", &(jet.matchedHadronFlavor),"matchedHadronFlavor[nref]/I");
	if(isMC){
		otree->Branch("ngj",  &jet.ngj);
		otree->Branch("refpt",    &(jet.refpt )        ,"refpt[ngj]/F");
		otree->Branch("geneta", &(jet.genjeteta)     ,"genjteta[ngj]/F");
		otree->Branch("genphi", &(jet.genjetphi)     ,"genjtphi[ngj]/F");
		otree->Branch("genpt" , &(jet.genjetpt)      ,"genjtpt[ngj]/F");
		otree->Branch("WTAgeneta",   &(jet.genjet_wta_eta),"WTAgenea[ngj]/F");
		otree->Branch("WTAgenphi",   &(jet.genjet_wta_phi),"WTAgenphi[ngj]/F");
	}
}
void forestSkimer::clearJetset(jetset &jet){
}

void forestSkimer::buildOuttree(){

	otree->Branch("vz",&(em->vz));
	otree->Branch("hiBin",&(em->hiBin));
	if(doTrk){
		otree->Branch("ntrk", &ntrk);
		otree->Branch("trkPt" ,    &(trkpt)     , "trkPt[ntrk]/F");
		otree->Branch("trkEta",    &(trketa)    , "trkEta[ntrk]/F");
		otree->Branch("trkPhi",    &(trkphi)    , "trkPhi[ntrk]/F");
		otree->Branch("highPurity",&(highPurity), "highPurity[ntrk]/O");
		otree->Branch("trkNdof",   &(trkndof)   , "trkNdof[ntrk]/b");
		otree->Branch("trkChi2",   &(trkchi2)   , "trkChi2[ntrk]/F");
		otree->Branch("trkNlayer", &(trknlayer) , "trkNlayer[ntrk]/b");
		otree->Branch("trkNHit",   &(trknhit)   , "trkNHit[ntrk]/b");
		otree->Branch("trkPtError",&(trkpterr)  , "trkPtError[ntrk]/F");
		otree->Branch("pfEcal",    &(pfEcal)    , "pfEcal[ntrk]/F");
		otree->Branch("pfHcal",    &(pfHcal)    , "pfHcal[ntrk]/F");
		if(isMC){
			otree->Branch("weight",&(em->weight));
			otree->Branch("pthat",&(em->pthat));
			//for gen particles
			otree->Branch("pt",  &gpptp);
			otree->Branch("eta", &gpetap);
			otree->Branch("phi", &gpphip);
			otree->Branch("chg", &gpchgp);
			otree->Branch("pdg", &gppdgIDp);
		}
	}

}

void forestSkimer::clearTrk(){
	gpptp.clear();
	gpetap.clear();
	gpphip.clear();
	gppdgIDp.clear();
	gpchgp.clear();
}


#endif
