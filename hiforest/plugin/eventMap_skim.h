
#ifndef EVENTMAP_AA2018_H
#define EVENTMAP_AA2018_H


//corresponding to 2017-2018 hiForest setup

#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <iostream>

// the flavor for B is determined by the matchedHadron flavor instead of the older version of refparton_flavorForB, switch to older version by turn AASetup = 0;
//

class eventMap  {
	public : 
		eventMap();
		eventMap(TFile*f){_file=f;};
		~eventMap(){
			std::cout<<"deleting the eventMap"<<std::endl;
			evtTree=nullptr;
			//evtTree->ResetBranchAddresses();
			std::cout<<"eventMap: deleted."<<std::endl;
		};
		void init();
		void loadFile( TFile *f){_file = f;};
		void loadJet(const char* name);
		void loadBTagger();
		void getEvent(Long64_t j){evtTree->GetEntry(j);};
		void loadTrack();
		void loadGenParticle();
		void unloadGP();
		void regEventFilter(int nfilter, std::string *filtername);
		void regEventFilter(std::vector<std::string> &filtername);
		bool checkEventFilter(){
			//mScrapingFilterreturn 1 for event needs to be skipped
			for(auto & it : filters) if(!it) return 1;
			return 0;
		}
		int nGP(){return gpptp->size();};
		int nJet(){return njet;};
		int nGenJet(){return ngj;};
		int nTrk(){return ntrk;};
		float gppt(int j) {return gpptp->at(j);}
		float gpeta(int j) {return gpetap->at(j);}
		float gpphi(int j) {return gpphip->at(j);}
		int gpchg(int j) {return gpchgp->at(j);}
		int gppdgID(int j) {return gppdgIDp->at(j);}
		TTree *hltTree, *filterTree, *trkTree, *gpTree, *jetTree=nullptr;
		TTree *evtTree; 
		TFile *_file = 0;
		std::vector<Int_t> filters;

		// detect if is MC by checking the if Gen particle exists.
		bool isMC = 0;
		// pp or PbPb setup
		bool AASetup = 0;
		//trk part
		static const int trkMax = 9999;
		int ntrk=0;
		Float_t trkpt[trkMax],trkpterr[trkMax],trketa[trkMax],trkphi[trkMax],trkchi2[trkMax];
		UChar_t trknlayer[trkMax], trknhit[trkMax], trkndof[trkMax];
		Bool_t highPurity[trkMax];
		Float_t pfEcal[trkMax], pfHcal[trkMax];

		//evt info
		Float_t weight = 0, vz = 0, pthat = 0;
		Int_t hiBin = 0;
		//gen particle
		int ngp = 0;
		std::vector<float> *gpptp=0, *gpetap=0, *gpphip=0;
		std::vector<int>  *gppdgIDp=0, *gpchgp=0;

		//jet set
		static const int jetMax = 999;
		int njet=0, ngj = 0;
		Float_t jetpt[jetMax],jeteta[jetMax],jetphi[jetMax],jet_wta_eta[jetMax],jet_wta_phi[jetMax], ref_jetpt[jetMax];
		Int_t flavor_forb[jetMax], jetTrkMax[jetMax];
		Float_t genjetpt[jetMax],genjeteta[jetMax],genjetphi[jetMax],genjet_wta_eta[jetMax],genjet_wta_phi[jetMax];
		Int_t genMatchIndex[jetMax];
		Float_t disc_csvV2[jetMax];

		// jet for CSV inputs
		Float_t pdisc_csvV2[jetMax];
		Float_t ndisc_csvV2[jetMax];
		Int_t nsvtx[jetMax];
		Int_t svtxntrk[jetMax];
		Float_t svtxdl[jetMax];
		Float_t svtxdls[jetMax];
		Float_t svtxm[jetMax];
};

eventMap::eventMap(){
}


void eventMap::init(){
	evtTree = (TTree*) _file->Get("mixing_tree");
	evtTree->SetBranchAddress("vz", &vz);
	evtTree->SetBranchAddress("hiBin", &hiBin);
	if(isMC){
		evtTree->SetBranchAddress("pthat", &pthat);
		evtTree->SetBranchAddress("weight", &weight);
	}
}

void eventMap::regEventFilter(int nfilter, std::string *filtername){
	filters.clear();
	filters.resize(nfilter);
	for(int i=0;i<nfilter; ++i){
		if(filterTree->GetLeaf(filtername[i].c_str())){
			evtTree->SetBranchAddress(filtername[i].c_str(), &(filters[i]));
		}else{
			std::cout<<"Error: No such filter name exits: "<<filtername[i]<<std::endl;
			return;
		}
	}
}

void eventMap::regEventFilter(std::vector<std::string> &filtername){
	filters.resize(filtername.size());
	int n = filtername.size();
	for(int i=0;i<n; ++i){
		if(filterTree->GetLeaf(filtername[i].c_str())){
			evtTree->SetBranchAddress(filtername[i].c_str(), &(filters[i]));
		}else{
			std::cout<<"Error: No such filter name exits: "<<filtername[i]<<std::endl;
			return;
		}
	}
}

void eventMap::loadTrack(){
	evtTree->SetBranchAddress("ntrk", &ntrk);
	evtTree->SetBranchAddress("trkPt", 	&trkpt);
	evtTree->SetBranchAddress("trkEta",	&trketa);
	evtTree->SetBranchAddress("trkPhi",	&trkphi);
	evtTree->SetBranchAddress("trkPtError", &trkpterr);
	evtTree->SetBranchAddress("trkChi2", 	&trkchi2);
	evtTree->SetBranchAddress("trkNlayer", 	&trknlayer);
	evtTree->SetBranchAddress("trkNHit", 	&trknhit);
	evtTree->SetBranchAddress("trkNdof", 	&trkndof);
	evtTree->SetBranchAddress("highPurity", &highPurity);
	evtTree->SetBranchAddress("pfEcal", 	&pfEcal);
	evtTree->SetBranchAddress("pfHcal", 	&pfHcal);
}

void eventMap::loadGenParticle(){
	evtTree->SetBranchAddress("pt",  &gpptp);
	evtTree->SetBranchAddress("eta", &gpetap);
	evtTree->SetBranchAddress("phi", &gpphip);
	evtTree->SetBranchAddress("chg", &gpchgp);
	evtTree->SetBranchAddress("pdg", &gppdgIDp);
}

void eventMap::unloadGP(){
	evtTree->ResetBranchAddresses();
}

void eventMap::loadJet(const char* name){
	evtTree->SetBranchAddress("nref", &njet);
	evtTree->SetBranchAddress("jtpt", &jetpt);
	evtTree->SetBranchAddress("jteta", &jeteta);
	evtTree->SetBranchAddress("jtphi", &jetphi);
	evtTree->SetBranchAddress("WTAeta", &jet_wta_eta);
	evtTree->SetBranchAddress("WTAphi", &jet_wta_phi);
	evtTree->SetBranchAddress("discr_csvV2", &disc_csvV2);
	evtTree->SetBranchAddress("trackMax", &jetTrkMax);
	if(isMC){
		evtTree->SetBranchAddress("genmatchindex", &genMatchIndex);// for reco jets
		if(AASetup) evtTree->SetBranchAddress("matchedHadronFlavor", &flavor_forb);// for reco jets
		else evtTree->SetBranchAddress("refparton_flavorForB", &flavor_forb);// for reco jets
		evtTree->SetBranchAddress("ngen", &ngj);
		evtTree->SetBranchAddress("refpt", &ref_jetpt);
		evtTree->SetBranchAddress("genpt", &genjetpt);
		evtTree->SetBranchAddress("geneta", &genjeteta);
		evtTree->SetBranchAddress("genphi", &genjetphi);
		evtTree->SetBranchAddress("WTAgeneta", &genjet_wta_eta);
		evtTree->SetBranchAddress("WTAgenphi", &genjet_wta_phi);
	}
}

void eventMap::loadBTagger(){
	evtTree->SetBranchAddress("pdiscr_csvV2", &pdisc_csvV2);
	evtTree->SetBranchAddress("ndiscr_csvV2", &ndisc_csvV2);
	evtTree->SetBranchAddress("nsvtx", &nsvtx);
	evtTree->SetBranchAddress("svtxntrk", &svtxntrk);
	evtTree->SetBranchAddress("svtxdl", &svtxdl);
	evtTree->SetBranchAddress("svtxdls", &svtxdls);
	evtTree->SetBranchAddress("svtxm", &svtxm);
}

#endif
