

#ifndef BTAGGERANALYZER_H
#define BTAGGERANALYZER_H

#include "treeScanner.h"
#include "Utility.h"

class bTaggerAnalyzer: public scanPlugin{
	enum flavorID {g, uds, c, b, unknown};
	public : 
	bTaggerAnalyzer(TString jetset): scanPlugin(), js_name(jetset){};
	~bTaggerAnalyzer(){}
	flavorID flavor2ID(int flavor){
		if(flavor == 0) return flavorID::g;
		else if(TMath::Abs(flavor) <= 3) return flavorID::uds;
		else if(TMath::Abs(flavor) == 4) return flavorID::c;
		else if(TMath::Abs(flavor) == 5) return flavorID::b;
		else return flavorID::unknown;
	}
	virtual void beginJob();
	virtual void endJob();
	virtual void run();

	bool (*recoJetCut)(eventMap*em, int j) = 0; // return 1 to skip;

	centralityHelper *cent=nullptr;
	TH2D** pdisc, **ndisc, **disc;
	TString js_name;
};


void bTaggerAnalyzer::beginJob(){
	hm = new histManager();
	em->loadJet(js_name);
	em->loadBTagger();
	int ncent = cent->nbins;
	pdisc = new TH2D*[ncent];
	ndisc = new TH2D*[ncent];
	disc  = new TH2D*[ncent];
	for(int i=0; i<ncent; ++i){
		TString centl  = cent->centLabel[i];
		pdisc[i]=hm->regHist<TH2D>(Form("pTagger_C%d", i), "positive tagger "+centl, 110, 0, 1.1, 5, 0, 5);
		ndisc[i]=hm->regHist<TH2D>(Form("nTagger_C%d", i), "negative tagger "+centl, 110, 0, 1.1, 5, 0, 5);
		disc [i]=hm->regHist<TH2D>(Form("wTagger_C%d", i), "working tagger "+centl, 110, 0, 1.1, 5, 0, 5);
	}
};

void bTaggerAnalyzer::run(){
	int jcent = cent->jcent(em->hiBin);
	float evtW= em->isMC ? em->weight : 1;
	for(int i=0; i< em->nJet(); ++i){
		if(recoJetCut(em, i)) continue;
		pdisc[jcent]->Fill(em->pdisc_csvV2[i], evtW);	
		ndisc[jcent]->Fill(em->ndisc_csvV2[i], evtW);	
		disc[jcent]->Fill(em->disc_csvV2[i], evtW);	
	}
}

void bTaggerAnalyzer::endJob(){
	auto wf = TFile::Open("bTagger_scan.root", "recreate");
	hm->write();
	wf->Close();
}

#endif
