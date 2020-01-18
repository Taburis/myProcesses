
#ifndef EVENTMAP_SKIM_H
#define EVENTMAP_SKIM_H

#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <iostream>

class eventMap{
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
//		void unloadGP();
		void regEventFilter(int nfilter, std::string *filtername);
		void regEventFilter(std::vector<std::string> &filtername);
		bool checkEventFilter(){
			//mScrapingFilterreturn 1 for event needs to be skipped
			for(auto & it : filters) if(!it) return 1;
			return 0;
		}
		int nGP(){return gpptp->size();};
		int nJet(){return jetpt->size();};
		int nGenJet(){return genjetpt->size();};
		int nTrk(){return trkpt->size();};
		float gppt(int j) {return gpptp->at(j);}
		float gpeta(int j) {return gpetap->at(j);}
		float gpphi(int j) {return gpphip->at(j);}
		int gpchg(int j) {return gpchgp->at(j);}
		int gppdgID(int j) {return gppdgIDp->at(j);}
		TTree *hltTree, *filterTree, *trkTree, *gpTree, *jetTree=nullptr;
		TTree *evtTree; 
		TFile *_file = 0;
		float vz, pthat = -1, weight=1;
		int hiBin=-1;
		std::vector<Int_t> filters;

		std::vector<float> *gpptp=0,*gpphip=0,*gpetap=0;
		std::vector<int>*gppdgIDp=0,*gpchgp=0;
		std::vector<float> *trkpt=0,*trketa=0,*trkphi=0,*trkpterr=0,*trkchi2=0,*pfEcal=0,*pfHcal=0;
		std::vector<int>*highPurity=0,*trknhit=0,*trkndof=0,*trknlayer=0;

		std::vector<float>*pdisc_csvV2=0,*ndisc_csvV2=0,*disc_csvV2=0;
		std::vector<float>*jetpt=0,*jeteta=0,*jetphi=0,*jet_wta_eta=0,*jet_wta_phi=0,*ref_jetpt=0;
		std::vector<int>*flavor_forb=0,*jetTrkMax=0,*genMatchIndex=0;
		std::vector<float>*genjetpt=0,*genjeteta=0,*genjetphi=0,*genjet_wta_eta=0,*genjet_wta_phi=0;

		// detect if is MC by checking the if Gen particle exists.
		bool isMC = 0;
		// pp or PbPb setup
		bool AASetup = 0;

};

void eventMap::init(){
	evtTree = (TTree*) _file->Get("mixing_tree");
	evtTree->SetBranchAddress("pthat", &pthat);
	evtTree->SetBranchAddress("vz", &vz);
	evtTree->SetBranchAddress("hiBin", &hiBin);
	evtTree->SetBranchAddress("weight", &weight);
}

void eventMap::regEventFilter(int nfilter, std::string *filtername){
	filters.clear();
	filters.resize(nfilter);
	for(int i=0;i<nfilter; ++i){
		if(evtTree->GetLeaf(filtername[i].c_str())){
			evtTree->SetBranchAddress(filtername[i].c_str(), &(filters[i]));
		}else{
			std::cout<<"Error: No such filter name exits: "<<filtername[i]<<std::endl;
			return;
		}
	}
}

void eventMap::regEventFilter(std::vector<std::string> &filtername){
	filters.clear();
	filters.resize(filtername.size());
	int n = filtername.size();
	for(int i=0;i<n; ++i){
		if(evtTree->GetLeaf(filtername[i].c_str())){
			evtTree->SetBranchAddress(filtername[i].c_str(), &(filters[i]));
		}else{
			std::cout<<"Error: No such filter name exits: "<<filtername[i]<<std::endl;
			return;
		}
	}
}

void eventMap::loadTrack(){
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

void eventMap::loadJet(const char* name){
	evtTree->SetBranchAddress("jtpt", &jetpt);
	evtTree->SetBranchAddress("jteta", &jeteta);
	evtTree->SetBranchAddress("jtphi", &jetphi);
	evtTree->SetBranchAddress("WTAeta", &jet_wta_eta);
	evtTree->SetBranchAddress("WTAphi", &jet_wta_phi);
	evtTree->SetBranchAddress("discr_csvV2", &disc_csvV2);
	evtTree->SetBranchAddress("trackMax", &jetTrkMax);
	if(isMC){
		//evtTree->SetBranchAddress("genmatchindex", &genMatchIndex);// for reco jets
		if(AASetup) evtTree->SetBranchAddress("matchedHadronFlavor", &flavor_forb);// for reco jets
		else evtTree->SetBranchAddress("refparton_flavorForB", &flavor_forb);// for reco jets
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
	//evtTree->SetBranchAddress("nsvtx", &nsvtx);
	//evtTree->SetBranchAddress("svtxntrk", &svtxntrk);
	//evtTree->SetBranchAddress("svtxdl", &svtxdl);
	//evtTree->SetBranchAddress("svtxdls", &svtxdls);
	//evtTree->SetBranchAddress("svtxm", &svtxm);
}



#endif
