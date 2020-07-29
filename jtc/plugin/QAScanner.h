

#ifndef QAScanner_H
#define QAScanner_H

#include "myProcesses/jtc/plugin/treeScanner.h"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/plugin/Utility.h"

class QAScanner : public scanPlugin{
	public : 
	QAScanner(TString name): scanPlugin(), ana_name(name){};

	bool (*recoJtCuts)(eventMap *em, int j) = 0;
	void book_evtInfo_hist();
	void addHiBins(int n, float *bins){ch = new centralityHelper(n, bins); ispp=0;}
	virtual void beginJob();
	virtual void endJob();
	virtual void run();

	centralityHelper * ch;
	int nhibin=1;

	bool ispp=1, isMC=0;
	TH1D **hpthat, **hvz;
	TH1D **hjetpt;
	const float jetptbin [29] = {60, 65, 70, 75,80,85, 90, 100, 110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360, 380, 400, 432, 500};
	TH1D *hcent;
	TString ana_name;
	TString ouptut = "result_qaScan.root";
};

void QAScanner::book_evtInfo_hist(){
	hcent = hm->regHist<TH1D>("hibin","", 200, 0, 200);
	hpthat = new TH1D*[nhibin];
	hvz = new TH1D*[nhibin];
	hjetpt = new TH1D*[nhibin];
	for(int i=0; i<nhibin; ++i){
		TString title = ispp ? "pp" : ch->centLabel[i];
		hpthat[i] = hm->regHist<TH1D>(Form("evtInfo/pthat_C%d",i),"pthat: "+title,100, 0,400);
		hvz[i] = hm->regHist<TH1D>(Form("evtInfo/vz_C%d",i),"vz: "+title, 72, -18,18);
		hjetpt[i] = hm->regHist<TH1D>(Form("recoJet/jetpt_C%d",i),"reco. Jet pT: "+title, 28, jetptbin);
	}
}

void QAScanner::beginJob(){
	if(!ispp) ch->makeLabels();
	nhibin = ch->nbins;
	book_evtInfo_hist();
}

void QAScanner::run(){
	// reco jet loop
	int jcent = 0;
	float weight = getEvtWeight();
	if(!ispp) jcent = ch->jcent(em->hiBin);

	hpthat[jcent]->Fill(em->pthat, weight);
	hvz[jcent]->Fill(em->vz, weight);
	hcent->Fill(em->hiBin, weight);

	for(int i=0; i<em->nJet();++i){
		hjetpt[jcent]->Fill(em->jetpt[i], weight);
	}


	// gen jet loop
	//for(int i=0; i<em->nGenJet();++i){
	//}
}

void QAScanner::endJob(){
}


#endif 
