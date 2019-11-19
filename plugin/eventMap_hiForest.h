
#ifndef EVENTMAP_H
#define EVENTMAP_H


//corresponding to 2017-2018 hiForest setup

class eventMap {
	public :
		eventMap();
		~eventMap(){};
		void loadFile( TFile *f){_file = f;};
		void addJet(TString, bool);
		void loadTrack();
		TTree *hltTree, *filterTree, *trkTree;
		TTree *evtTree; 
		TFile *_file = 0;

		//trk part
		const trkMax = 9999;
		Float_t trkpt[trkMax],trkpterr[trkMax],trketa[trkMax],trkphi[trkMax],trkchi2[trkMax];
		UChar_t trknlayer[trkMax], trknhit[trkMax], trkndof[trkMax];
		Bool_t highPurity;

		//gen particle
		Float_t gppt[trkMax], gpeta[trkMax], gpphi[trkMax];
		Int_t  gppdgID[trkMax], gpchg[trkMax];
};

eventMap::eventMap(){
	evtTree = new TChain("hiEvtAnalyzer", "");
}

void eventMap::loadTrack(){
	trkTree = (TTree*) _file->Get("ppTrack/trackTree");
	evtTree->AddFriend(trkTree);
	evtTree->SetBranchAddress("trkPt", &trkpt
}


#endif
