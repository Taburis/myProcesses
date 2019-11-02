

#ifndef EVENTMAP_H
#define EVENTMAP_H

#include "eventMapBase.h"

//corresponding to 2017-2018 hiForest setup

class eventMap : public eventMapBase{
		public : eventMap();
				 ~eventMap(){};
				 void loadFile( TFile *f){_file = f;};
				 void addJet(TString, bool);
				 void addTrack();
				 TTree *hltTree, *filterTree, *trkTree;
				 TChain *evtTree; 
				 TFile *_file = 0;
};

eventMap::eventMap(){
		evtTree = new TChain("hiEvtAnalyzer", "");
}

void eventMap::addTrack(){
		trkTree = new Tree("ppTrack", "");
		evtTree->AddFriend(trkTree);
}


#endif
