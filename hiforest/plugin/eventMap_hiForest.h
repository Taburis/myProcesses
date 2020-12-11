
#ifndef EVENTMAP_AA2018_H
#define EVENTMAP_AA2018_H


//corresponding to 2017-2018 hiForest setup

#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <iostream>

//

class eventMap  {
	public : 
		eventMap(){};
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
		void loadJetID();
		void loadBTagger();
		void loadBTaggerInputVariables();
		void getEvent(Long64_t j){evtTree->GetEntry(j);};
		void loadTrack();
		void loadMuons(bool fullInfo = 0);
		void loadGenParticle();
		void unloadGP();
		void addBTVInfo();
		void loadTriggerInfo(int ntrig, std::string *trigs);
		void loadTriggerInfo(std::vector<std::string> &trigs);
		void copyTriggerInfo(TTree* t, int ntrig, std::string *trigs);
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
		int gpIsStable(int j) {if(gpStableTag->at(j)==1) return 1; return 0;}
		int gpSube(int j){ return gpsube->at(j);}

		TTree *hltTree, *filterTree, *trkTree, *gpTree,*EGTree, *jetTree=nullptr;
		TTree *evtTree; 
		TFile *_file = 0;
		std::vector<Int_t> filters;

		// detect if is MC by checking the if Gen particle exists.
		bool isMC = 0, isHI =0;
		bool addJetID = 0;
		// pp or PbPb setup
		//trk part
		static const int trkMax = 99999;
		int ntrk=0;
		Float_t trkpt[trkMax],trkpterr[trkMax],trketa[trkMax],trkphi[trkMax],trkchi2[trkMax];
		UChar_t trknlayer[trkMax], trknhit[trkMax], trkndof[trkMax];
		Bool_t highPurity[trkMax];
		Float_t pfEcal[trkMax], pfHcal[trkMax];

		// trigger bits
		Int_t *trigFlag=0;
		//muons
		Int_t          nMuon;
		std::vector<float>  *muonPt      =0;
		std::vector<float>  *muonEta     =0;
		std::vector<float>  *muonPhi     =0;
		std::vector<int>    *muonCharge  =0;
		std::vector<int>    *muonType    =0;
		std::vector<int>    *muonIsGood  =0;

		std::vector<int>    *muonIsGlobal=0;
		std::vector<int>    *muonIsTracker=0;
		std::vector<int>    *muonIsPF    =0;
		std::vector<int>    *muonIsSTA   =0;

		std::vector<float>  *muonD0      =0;
		std::vector<float>  *muonDz      =0;
		std::vector<float>  *muonD0Err   =0;
		std::vector<float>  *muonDzErr   =0;
		std::vector<float>  *muonIP3D    =0;
		std::vector<float>  *muonIP3DErr =0;
		std::vector<float>  *muonChi2NDF =0;

		std::vector<float>  *muonInnerD0 =0;
		std::vector<float>  *muonInnerDz =0;
		std::vector<float>  *muonInnerD0Err=0;
		std::vector<float>  *muonInnerDzErr=0;
		std::vector<float>  *muonInnerPt =0;
		std::vector<float>  *muonInnerPtErr =0;
		std::vector<float>  *muonInnerEta   =0;

		std::vector<int>    *muonTrkLayers  =0;
		std::vector<int>    *muonPixelLayers=0;
		std::vector<int>    *muonPixelHits  =0;
		std::vector<int>    *muonMuonHits   =0;
		std::vector<int>    *muonTrkQuality =0;
		std::vector<int>    *muonStations   =0;
		std::vector<float>  *muonIsoTrk     =0;
		std::vector<float>  *muonPFChIso    =0;
		std::vector<float>  *muonPFPhoIso   =0;
		std::vector<float>  *muonPFNeuIso   =0;
		std::vector<float>  *muonPFPUIso    =0;
		std::vector<int>    *muonIDSoft     =0;
		std::vector<int>    *muonIDLoose    =0;
		std::vector<int>    *muonIDMedium   =0;
		std::vector<int>    *muonIDMediumPrompt=0;
		std::vector<int>    *muonIDTight    =0;
		std::vector<int>    *muonIDGlobalHighPt=0;
		std::vector<int>    *muonIDTrkHighPt=0;
		std::vector<int>    *muonIDInTime   =0;


		//evt info
		Float_t weight = 0, vz = 0, pthat = 0;
		Int_t hiBin = 0;
		//gen particle
		int ngp = 0;
		bool stableOnly = 1;
		std::vector<float> *gpptp=0, *gpetap=0, *gpphip=0;
		std::vector<int>  *gppdgIDp=0, *gpchgp=0, *gpsube=0, *gpStableTag=0;

		//jet set
		static const int jetMax = 9999;
		int njet=0, ngj = 0;
		Float_t jetpt[jetMax],jeteta[jetMax],jetphi[jetMax],jet_wta_eta[jetMax],jet_wta_phi[jetMax], ref_jetpt[jetMax];
		Int_t flavor_forb[jetMax], bHadronNumber[jetMax];
		Int_t matchedHadronFlavor[jetMax], matchedPartonFlavor[jetMax];
		Float_t genjetpt[jetMax],genjeteta[jetMax],genjetphi[jetMax],genjet_wta_eta[jetMax],genjet_wta_phi[jetMax];
		Int_t genMatchIndex[jetMax];
		Float_t disc_csvV2[jetMax];

		// jet for CSV inputs
		Float_t pdisc_csvV2[jetMax];
		Float_t ndisc_csvV2[jetMax];
		Int_t nsvtx[jetMax];
		static const int csv_sv_max = 30000;
		Int_t svtxntrk[csv_sv_max];
		Float_t svtxdl[csv_sv_max];
		Float_t svtxdls[csv_sv_max];
		Float_t svtxm[csv_sv_max];

		//jetID
		Float_t trkPtMax[jetMax], trkPtSum[jetMax];
		Int_t trkNumber[jetMax]; 

		Float_t chargedPfPtMax[jetMax], chargedPfPtSum[jetMax];
		Int_t chargedPfNumber[jetMax];

		Float_t neutralPfPtMax[jetMax], neutralPfPtSum[jetMax];
		Int_t neutralPfNumber[jetMax];

		Float_t ePtMax[jetMax], ePtSum[jetMax]; Int_t eNumber[jetMax];

		Float_t photonPtMax[jetMax], photonPtSum[jetMax]; Int_t photonNumber[jetMax];

		Float_t muonPtMax[jetMax], muonPtSum[jetMax]; Int_t muonNumber[jetMax];
		Float_t hcalSum[jetMax], ecalSum[jetMax];

		//CSV input variables;
		Float_t csv_trkMul[jetMax];
		Float_t csv_trkIndexStart[jetMax], csv_trkIndexEnd[jetMax];
		static const int csv_trk_max = 100000;
		Float_t csv_trk3dIPSig[csv_trk_max], csv_trk3dIP[csv_trk_max], csv_trkDist[csv_trk_max];
		Float_t  csv_trkPtRel[csv_trk_max], csv_trkMomentum[csv_trk_max],csv_trkDr[csv_trk_max];

		//V0, GSP information
		static const int nv0 = 5000;
		bool GSPevt=0, doBtag = 0;
		int ngenV0 =0;
		float genV0_pt[nv0], genV0_eta[nv0], genV0_phi[nv0], genV0_SVx[nv0], genV0_SVy[nv0], genV0_SVz[nv0];
		int genV0_pdgID[nv0], genV0_ncharged[nv0];
};


void eventMap::init(){
	evtTree = (TTree*) _file->Get("hiEvtAnalyzer/HiTree");
	evtTree->SetBranchAddress("vz", &vz);
	evtTree->SetBranchAddress("hiBin", &hiBin);
	if(isMC){
		evtTree->SetBranchAddress("pthat", &pthat);
		evtTree->SetBranchAddress("weight", &weight);
	}
}

void eventMap::loadTriggerInfo(int ntrig, std::string *trigs){
	hltTree = (TTree*) _file->Get("hltanalysis/HltTree");
	evtTree->AddFriend(hltTree);
	trigFlag = new Int_t[ntrig];
	for(int i=0;i<ntrig; ++i){
		if(hltTree->GetLeaf(trigs[i].c_str())){
			evtTree->SetBranchAddress(trigs[i].c_str(), &(trigFlag[i]));
		}
	}
}

void eventMap::loadTriggerInfo(std::vector<std::string> &trigs){
	hltTree = (TTree*) _file->Get("hltanalysis/HltTree");
	evtTree->AddFriend(hltTree);
	int ntrig = trigs.size();
	trigFlag = new Int_t[ntrig];
	for(int i=0;i<ntrig; ++i){
		if(hltTree->GetLeaf(trigs[i].c_str())){
			evtTree->SetBranchAddress(trigs[i].c_str(), &(trigFlag[i]));
		}
	}
}

void eventMap::copyTriggerInfo(TTree* t, int ntrig, std::string *trigs){
	for(int i=0;i<ntrig; ++i){
		if(evtTree->GetLeaf(trigs[i].c_str())){
			t->Branch(trigs[i].c_str(), &(trigFlag[i]));
		}
	}
}

void eventMap::regEventFilter(int nfilter, std::string *filtername){
	filterTree = (TTree*) _file->Get("skimanalysis/HltTree");
	evtTree->AddFriend(filterTree);
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
	filterTree = (TTree*) _file->Get("skimanalysis/HltTree");
	evtTree->AddFriend(filterTree);
	filters.clear();
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
	trkTree = (TTree*) _file->Get("ppTrack/trackTree");
	//trkTree = new Tree("ppTrack", "");
	evtTree->AddFriend(trkTree);
	evtTree->SetBranchAddress("nTrk", &ntrk);
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
	gpTree = (TTree*) _file->Get("HiGenParticleAna/hi");
	evtTree->AddFriend(gpTree);
	evtTree->SetBranchAddress("pt",  &gpptp);
	evtTree->SetBranchAddress("eta", &gpetap);
	evtTree->SetBranchAddress("phi", &gpphip);
	evtTree->SetBranchAddress("chg", &gpchgp);
	evtTree->SetBranchAddress("pdg", &gppdgIDp);
	if(isHI) evtTree->SetBranchAddress("sube",&gpsube);
	if(!stableOnly) evtTree->SetBranchAddress("sta",&gpStableTag);
}

void eventMap::unloadGP(){
	evtTree->ResetBranchAddresses();
}

void eventMap::addBTVInfo(){
	evtTree->SetBranchAddress("GSPevt", &GSPevt);
	evtTree->SetBranchAddress("ngenV0", &ngenV0);
	evtTree->SetBranchAddress("genV0_pt",       &genV0_pt);
	evtTree->SetBranchAddress("genV0_eta",      &genV0_eta);
	evtTree->SetBranchAddress("genV0_phi",      &genV0_phi);
	evtTree->SetBranchAddress("genV0_pdgID",    &genV0_pdgID);
	evtTree->SetBranchAddress("genV0_ncharged", &genV0_ncharged);
	evtTree->SetBranchAddress("genV0_SVx", &genV0_SVx);
	evtTree->SetBranchAddress("genV0_SVy", &genV0_SVy);
	evtTree->SetBranchAddress("genV0_SVz", &genV0_SVz);
}

void eventMap::loadJet(const char* name){
	jetTree = (TTree*) _file->Get(Form("%s/t", name));		
	evtTree->AddFriend(jetTree);
	evtTree->SetBranchAddress("nref", &njet);
	evtTree->SetBranchAddress("jtpt", &jetpt);
	evtTree->SetBranchAddress("jteta", &jeteta);
	evtTree->SetBranchAddress("jtphi", &jetphi);
	evtTree->SetBranchAddress("WTAeta", &jet_wta_eta);
	evtTree->SetBranchAddress("WTAphi", &jet_wta_phi);
	evtTree->SetBranchAddress("discr_csvV2", &disc_csvV2);
	if(isMC){
		evtTree->SetBranchAddress("genmatchindex", &genMatchIndex);// for reco jets
		if(isHI) evtTree->SetBranchAddress("refparton_flavorForB", &flavor_forb);// for reco jets
		else evtTree->SetBranchAddress("jtHadronFlavor", &flavor_forb);// for reco jets
		evtTree->SetBranchAddress("matchedHadronFlavor", &matchedHadronFlavor);// for reco jets
		evtTree->SetBranchAddress("matchedPartonFlavor", &matchedPartonFlavor);// for reco jets
		evtTree->SetBranchAddress("bHadronNumber", &bHadronNumber);
		evtTree->SetBranchAddress("ngen", &ngj);
		evtTree->SetBranchAddress("refpt", &ref_jetpt);
		evtTree->SetBranchAddress("genpt", &genjetpt);
		evtTree->SetBranchAddress("geneta", &genjeteta);
		evtTree->SetBranchAddress("genphi", &genjetphi);
		evtTree->SetBranchAddress("WTAgeneta", &genjet_wta_eta);
		evtTree->SetBranchAddress("WTAgenphi", &genjet_wta_phi);
	}
	if(doBtag) addBTVInfo();
	if(addJetID) loadJetID();
}

void eventMap::loadBTagger(){
	if(jetTree ==nullptr) {
		std::cout<<"Please load the jets by calling loadJet(jetset_name) before load the btagger for that certain jet set."<<std::endl;
		return;
	}	
	evtTree->SetBranchAddress("pdiscr_csvV2", &pdisc_csvV2);
	evtTree->SetBranchAddress("ndiscr_csvV2", &ndisc_csvV2);
	evtTree->SetBranchAddress("nsvtx", &nsvtx);
	evtTree->SetBranchAddress("svtxntrk", &svtxntrk);
	evtTree->SetBranchAddress("svtxdl", &svtxdl);
	evtTree->SetBranchAddress("svtxdls", &svtxdls);
	evtTree->SetBranchAddress("svtxm", &svtxm);
}

void eventMap::loadBTaggerInputVariables(){
	//	evtTree->SetBranchAddress("TagVarCSV_jetNSecondaryVertices", &csv_jetNSV);
	evtTree->SetBranchAddress("Jet_nFirstTrkTagVarCSV", &csv_trkIndexStart);
	evtTree->SetBranchAddress("Jet_nLastTrkTagVarCSV", &csv_trkIndexEnd);
	evtTree->SetBranchAddress("TagVarCSV_jetNTracks", &csv_trkMul);
	evtTree->SetBranchAddress("TagVarCSV_trackSip3dVal", &csv_trk3dIP);
	evtTree->SetBranchAddress("TagVarCSV_trackSip3dSig", &csv_trk3dIPSig);
	evtTree->SetBranchAddress("TagVarCSV_trackJetDistVal", &csv_trkDist);
	evtTree->SetBranchAddress("TagVarCSV_trackPtRel", &csv_trkPtRel);
	evtTree->SetBranchAddress("TagVarCSV_trackMomentum", &csv_trkMomentum);
	evtTree->SetBranchAddress("TagVarCSV_trackDeltaR", &csv_trkDr);
}

void eventMap::loadJetID(){
	evtTree->SetBranchAddress("trackMax", &trkPtMax);
	evtTree->SetBranchAddress("trackSum", &trkPtSum);
	evtTree->SetBranchAddress("trackN",   &trkNumber);
	evtTree->SetBranchAddress("chargedMax", &chargedPfPtMax);
	evtTree->SetBranchAddress("chargedSum", &chargedPfPtSum);
	evtTree->SetBranchAddress("chargedN", &chargedPfNumber);
	evtTree->SetBranchAddress("neutralMax", &neutralPfPtMax);
	evtTree->SetBranchAddress("neutralSum", &neutralPfPtSum);
	evtTree->SetBranchAddress("neutralN", &neutralPfNumber);
	evtTree->SetBranchAddress("eMax", &ePtMax);
	evtTree->SetBranchAddress("eSum", &ePtSum);
	evtTree->SetBranchAddress("eN",   &eNumber);
	evtTree->SetBranchAddress("photonMax", &photonPtMax);
	evtTree->SetBranchAddress("photonSum", &photonPtSum);
	evtTree->SetBranchAddress("photonN",   &photonNumber);
	evtTree->SetBranchAddress("muMax", &muonPtMax);
	evtTree->SetBranchAddress("muSum", &muonPtSum);
	evtTree->SetBranchAddress("muN",   &muonNumber);
	evtTree->SetBranchAddress("hcalSum",   &hcalSum);
	evtTree->SetBranchAddress("ecalSum",   &ecalSum);
}

void eventMap::loadMuons(bool fullInfo){

	EGTree = (TTree*) _file->Get("ggHiNtuplizerGED/EventTree");		
	evtTree->AddFriend(EGTree);

	evtTree->SetBranchAddress("nMu", &nMuon);
	evtTree->SetBranchAddress("muPt",  &muonPt);
	evtTree->SetBranchAddress("muEta", &muonEta);
	evtTree->SetBranchAddress("muPhi", &muonPhi);
	evtTree->SetBranchAddress("muCharge", &muonCharge);
	evtTree->SetBranchAddress("muType", &muonType);

	evtTree->SetBranchAddress("muIsGood", &muonIsGood);
	evtTree->SetBranchAddress("muIsGlobal", &muonIsGlobal);
	evtTree->SetBranchAddress("muIsTracker", &muonIsTracker);
	evtTree->SetBranchAddress("muIsPF", &muonIsPF);
	evtTree->SetBranchAddress("muIsSTA", &muonIsSTA);

	evtTree->SetBranchAddress("muD0", &muonD0);
	evtTree->SetBranchAddress("muDz", &muonDz);
	evtTree->SetBranchAddress("muIP3D", &muonIP3D);
	evtTree->SetBranchAddress("muIP3DErr", &muonIP3DErr);
	evtTree->SetBranchAddress("muD0Err", &muonD0Err);
	evtTree->SetBranchAddress("muDzErr", &muonDzErr);
	evtTree->SetBranchAddress("muChi2NDF", &muonChi2NDF);
	evtTree->SetBranchAddress("muStations",   &muonStations);
	evtTree->SetBranchAddress("muTrkLayers",  &muonTrkLayers);
	evtTree->SetBranchAddress("muPixelLayers",&muonPixelLayers);
	evtTree->SetBranchAddress("muMuonHits",   &muonMuonHits);
	evtTree->SetBranchAddress("muTrkQuality", &muonTrkQuality);

	if(!fullInfo) return;
	evtTree->SetBranchAddress("muIDSoft",   &muonIDSoft);
	evtTree->SetBranchAddress("muIDLoose",  &muonIDLoose);
	evtTree->SetBranchAddress("muIDMedium", &muonIDMedium);
	evtTree->SetBranchAddress("muIDTight", &muonIDTight);
	evtTree->SetBranchAddress("muIDGlobalHighPt", &muonIDGlobalHighPt);
	evtTree->SetBranchAddress("muIDTrkHighPt", &muonIDTrkHighPt);
	evtTree->SetBranchAddress("muIDMediumPrompt", &muonIDMediumPrompt);
	evtTree->SetBranchAddress("muIDInTime", &muonIDInTime);

	evtTree->SetBranchAddress("muInnerD0", &muonInnerD0);
	evtTree->SetBranchAddress("muInnerDz", &muonInnerDz);
	evtTree->SetBranchAddress("muInnerD0Err", &muonInnerD0Err);
	evtTree->SetBranchAddress("muInnerDzErr", &muonInnerDzErr);
	evtTree->SetBranchAddress("muInnerPt",    &muonInnerPt);
	evtTree->SetBranchAddress("muInnerPtErr", &muonInnerPtErr);
	evtTree->SetBranchAddress("muInnerEta",   &muonInnerEta);

	evtTree->SetBranchAddress("muPixelHits",  &muonPixelHits);
	evtTree->SetBranchAddress("muIsoTrk",     &muonIsoTrk);
	evtTree->SetBranchAddress("muPFChIso",    &muonPFChIso);
	evtTree->SetBranchAddress("muPFPhoIso",   &muonPFPhoIso);
	evtTree->SetBranchAddress("muPFNeuIso",   &muonPFNeuIso);
	evtTree->SetBranchAddress("muPFPUIso",    &muonPFPUIso);


}

#endif
