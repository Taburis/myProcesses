
#ifndef jetRecoAnalyzer_H
#define jetRecoAnalyzer_H

#include "myProcesses/jtc/plugin/treeScanner.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

class jetRecoAnalyzer: public scanPlugin{
	public : 
		jetRecoAnalyzer(TString name): scanPlugin(), ana_name(name){
		};
		~jetRecoAnalyzer(){};
		virtual void beginJob();
		virtual void endJob();
		virtual void run();
		// for Y bin: 0 inclusive jet, 1 tagged jet
		TH1D **hrandmCone;
		TH1D **hjtCone;
		TH1D* hvz, *hpthat, *hcent;
		TString js_name;
		TString ana_name;
		centralityHelper *cent=nullptr;
		std::vector<Double_t> jtpt, jteta, jtphi; // for random cone
};

void jetRecoAnalyzer::beginJob(){
	em->loadJet(js_name);
	hm = new histManager();
	int ncent = cent->nbins;
	hrandmCone = new TH1D*[ncent];
	hjtCone = new TH1D*[ncent];
	hvz = hm->regHist<TH1D>("hvz", "", 150, -15, 15);
	hcent = hm->regHist<TH1D>("hcent", "", 200, 0, 200);
	if(em->isMC){
		hpthat = hm->regHist<TH1D>("hpthat", "", 100, 0, 400);
	}
	for(int i=0; i<ncent; ++i){
		TString centl  = cent->centLabel[i];
		hrandmCone[i] = hm->regHist<TH1D>(Form("hrdmCone_C%d",i),"Random cone trkMul "+centl,50, 0, 50);
		hjtCone   [i] = hm->regHist<TH1D>(Form("hjtCone_C%d",i),"In jet cone trkMul "+centl,50, 0, 50);
	}
	jtpt.clear();
	jteta.clear();
	jtphi.clear();
	hm->sumw2();
}

void jetRecoAnalyzer::endJob(){
	auto wf = TFile::Open(ana_name+"_out.root", "recreate");
	hm->write(wf);
	wf->Close();
}

void jetRecoAnalyzer::run(){
	int jcent = cent->jcent(em->hiBin);
	if(jcent < 0) return;
	float evtW= em->isMC ? em->weight : 1;
	evtW = evtW*ts->evtW;
	hvz->Fill(Double_t(em->vz), evtW);
	hcent->Fill(Double_t(em->hiBin), evtW);
	if(em->isMC) hpthat->Fill(em->pthat, evtW);

	for(int i=0; i< jtpt.size(); ++i){
		bool doskip = 0;
		for(int k=0; k<em->nJet(); ++k){
			float dr = findDr(jteta[i],jtphi[i],em->jeteta[k],em->jetphi[k]);
			if(dr> 0.4){ doskip = 1; break;}
		}
		if(doskip) continue;
		for(int j=0; j< em->nTrk(); ++j){
			float dr = findDr(jteta[i],jtphi[i],em->trketa[j],em->trkphi[j]);
			if(dr > 0.4) continue;
			hrandmCone[jcent]->Fill(em->trkpt[i], evtW);
		}
	}	
	jtpt.clear();
	jtphi.clear();
	jteta.clear();
	for(int i=0; i< em->nJet(); ++i){
		//if(recoJetCut(em, i)) continue;
		jtpt .emplace_back(em->jetpt[i]);
		jtphi.emplace_back(em->jetphi[i]);
		jteta.emplace_back(em->jeteta[i]);
		for(int j=0; j< em->nTrk(); ++j){
			float dr = findDr(em->jeteta[i],em->jetphi[i],em->trketa[j],em->trkphi[j]);
			if(dr > 0.4) continue;
			hjtCone[jcent]->Fill(em->trkpt[i], evtW);
		}
	}
}

#endif 
