

#ifndef BTAGGERANALYZER_H
#define BTAGGERANALYZER_H

#include "treeScanner.h"
#include "Utility.h"
#include "jtcUti.h"
#include "plotLib.h"
#include "TH3D.h"

//corresponds to the AASetup eventMaps
class eventMap;
class bTaggerAnalyzer: public scanPlugin{
	public : 
		enum flavorID {udsg, c, b, unknown};
		bTaggerAnalyzer(TString name): scanPlugin(), ana_name(name){
			hm = new histManager();
		};
		~bTaggerAnalyzer(){}	
		void scheduleJetSet(TString name){js_name = name;}
		flavorID flavor2ID(int flavor){
			if(flavor == 0) return flavorID::udsg;
			else if(TMath::Abs(flavor) == 4) return flavorID::c;
			else if(TMath::Abs(flavor) == 5) return flavorID::b;
			else return flavorID::unknown;
		}
		virtual void beginJob();
		int allocateHists();
		virtual void endJob();
		virtual void run();
		void loadStep1File(TFile *f, bool isMC);
		void addProbeWPs(int n, Double_t *arr){
			doPtCSV=1;
			csvWPs=arr; nCSVWPs = n;
		}

		bool (*recoJetCut)(eventMap*em, int j) = 0; // return 1 to skip;

		bool isMC, doPtCSV;
		int ncent, nflavor = int(flavorID::unknown+1);
		Double_t flavorbin[5] = {-.5, .5, 1.5, 2.5, 3.5};
		centralityHelper *cent=nullptr;
		int nCSVWPs;
		Double_t * csvWPs;
		TH1D *hvz, *hpthat, *hcent;
		TH2D** pdisc, **ndisc, **disc, **jtpt, **jteta, **jtphi;
		TH2D** hnsvtx, **hsvtxm, **hsvtxdl, **hsvtxdls, **hsvtxntrk;
		TH3D** pTagger3D, **nTagger3D, **wTagger3D;
		TString js_name, ana_name;
		bTaggerAnalyzer *btgAna;
};

int bTaggerAnalyzer::allocateHists(){
	//return the # of centrailty bins
	ncent = cent->nbins;
	jtpt = new TH2D*[ncent];
	jteta = new TH2D*[ncent];
	jtphi = new TH2D*[ncent];
	pdisc = new TH2D*[ncent];
	ndisc = new TH2D*[ncent];
	disc  = new TH2D*[ncent];
	hnsvtx= new TH2D*[ncent];
	hsvtxm= new TH2D*[ncent];
	hsvtxdl= new TH2D*[ncent];
	hsvtxdls= new TH2D*[ncent];
	hsvtxntrk = new TH2D*[ncent];
	if(doPtCSV){
		pTagger3D = new TH3D*[ncent];
		nTagger3D = new TH3D*[ncent];
		wTagger3D = new TH3D*[ncent];
	}
	return ncent;
}

void bTaggerAnalyzer::beginJob(){
	em->loadJet(js_name);
	em->loadBTagger();
	// prepare the pointers for hists
	int ncent = allocateHists();
	hvz = hm->regHist<TH1D>("hvz", "", 150, -15, 15);
	hcent = hm->regHist<TH1D>("hcent", "", 200, 0, 200);
	if(em->isMC){
		hpthat = hm->regHist<TH1D>("hpthat", "", 100, 0, 400);
	}
	for(int i=0; i<ncent; ++i){
		TString centl  = cent->centLabel[i];
		jtpt[i]=hm->regHist<TH2D>(Form("jtpt_C%d", i), "jet pT distribution "+centl, default_setup::nptbin , default_setup::ptbin, 5, -0.5, 4.5);
		jteta[i]=hm->regHist<TH2D>(Form("jteta_C%d", i), "jet eta distribution "+centl, 50, -2.5, 2.5, 5, -.5, 4.5);
		jtphi[i]=hm->regHist<TH2D>(Form("jtphi_C%d", i), "jet phi distribution "+centl, 50, -TMath::Pi(), TMath::Pi(), 5, -.5, 4.5);
		pdisc[i]=hm->regHist<TH2D>(Form("pTagger_C%d", i), "positive tagger "+centl, 110, 0, 1.1, 5, -0.5, 4.5);
		ndisc[i]=hm->regHist<TH2D>(Form("nTagger_C%d", i), "negative tagger "+centl, 110, 0, 1.1, 5, -0.5, 4.5);
		disc [i]=hm->regHist<TH2D>(Form("wTagger_C%d", i), "working tagger "+centl, 110, 0, 1.1, 5, -.5, 4.5);
		hnsvtx [i]=hm->regHist<TH2D>(Form("hnsvtx_C%d", i), "# of SV "+centl, 20, 0, 20, 5, -.5, 4.5);
		hsvtxm [i]=hm->regHist<TH2D>(Form("hsvtxm_C%d", i), "SV mass "+centl, 20, 0, 10, 5, -.5, 4.5);
		hsvtxdl [i]=hm->regHist<TH2D>(Form("hsvtxdl_C%d", i), "SV distance "+centl, 60, 0, 3, 5, -0.5,4.5);
		hsvtxdls[i]=hm->regHist<TH2D>(Form("hsvtxdls_C%d", i), "SV distance significance "+centl, 200, 0, 200, 5, -0.5, 4.5);
		hsvtxntrk [i]=hm->regHist<TH2D>(Form("hsvtxntrk_C%d", i), "# of trks assoicated to SV "+centl, 30, 0, 30, 5, -0.5, 4.5);
		if(!doPtCSV) return;
		pTagger3D[i]=hm->regHist<TH3D>(Form("pTagger3D_C%d", i), "positive tagger "+centl, default_setup::nptbin , default_setup::ptbin, nCSVWPs, csvWPs, nflavor, flavorbin);
		nTagger3D[i]=hm->regHist<TH3D>(Form("nTagger3D_C%d", i), "positive tagger "+centl, default_setup::nptbin , default_setup::ptbin, nCSVWPs, csvWPs, nflavor, flavorbin);
		wTagger3D[i]=hm->regHist<TH3D>(Form("wTagger3D_C%d", i), "positive tagger "+centl, default_setup::nptbin , default_setup::ptbin, nCSVWPs, csvWPs, nflavor, flavorbin);
	}
};

void bTaggerAnalyzer::run(){
	int jcent = cent->jcent(em->hiBin);
	float evtW= em->isMC ? em->weight : 1;
	evtW = evtW*ts->evtW;
	hvz->Fill(Double_t(em->vz), evtW);
	hcent->Fill(Double_t(em->hiBin), evtW);
	if(em->isMC) hpthat->Fill(em->pthat, evtW);
	for(int i=0; i< em->nJet(); ++i){
		if(recoJetCut(em, i)) continue;
		if(em->isMC) if(em->ref_jetpt[i] < 50) continue;
		flavorID flavor = flavorID::unknown;
		if(em->isMC) flavor = flavor2ID(em->flavor_forb[i]);
		//	cout<<flavor<<endl;
		jtpt [jcent]->Fill(em->jetpt[i],flavor, evtW);	
		jteta[jcent]->Fill(em->jeteta[i],flavor, evtW);	
		jtphi[jcent]->Fill(em->jetphi[i],flavor, evtW);	
		pdisc[jcent]->Fill(em->pdisc_csvV2[i],flavor, evtW);	
		ndisc[jcent]->Fill(em->ndisc_csvV2[i],flavor, evtW);	
		disc[jcent]->Fill(em->disc_csvV2[i],flavor, evtW);	
		hnsvtx[jcent]->Fill(em->nsvtx[i],flavor, evtW);
		hsvtxntrk[jcent]->Fill(em->svtxntrk[i],flavor, evtW);
		hsvtxm[jcent]->Fill(em->svtxm[i],flavor, evtW);
		hsvtxdl[jcent]->Fill(em->svtxdl[i],flavor, evtW);
		hsvtxdls[jcent]->Fill(em->svtxdls[i],flavor, evtW);
		if(doPtCSV){
			pTagger3D[jcent]->Fill(em->jetpt[i], em->pdisc_csvV2[i], flavor,evtW);
			nTagger3D[jcent]->Fill(em->jetpt[i], em->ndisc_csvV2[i], flavor,evtW);
			wTagger3D[jcent]->Fill(em->jetpt[i], em->disc_csvV2[i], flavor,evtW);
		}
	}
}

void bTaggerAnalyzer::endJob(){
	auto wf = TFile::Open(ana_name+"_step1.root", "recreate");
	hm->write();
	wf->Close();
}

void bTaggerAnalyzer::loadStep1File(TFile *f, bool ismc){
	isMC = ismc;
	ncent = allocateHists();
	hvz = (TH1D*) f->Get("hvz");
	hcent = (TH1D*) f->Get("hcent");
	for(int i=0;i<ncent; ++i){
		jtpt     [i]= (TH2D*) f->Get(Form("jtpt_C%d", i));
		jteta     [i]= (TH2D*) f->Get(Form("jteta_C%d", i));
		jtphi     [i]= (TH2D*) f->Get(Form("jtphi_C%d", i));
		ndisc    [i]= (TH2D*) f->Get(Form("nTagger_C%d", i));
		pdisc    [i]= (TH2D*) f->Get(Form("pTagger_C%d", i));
		disc     [i]= (TH2D*) f->Get(Form("wTagger_C%d", i));
		hnsvtx   [i]= (TH2D*) f->Get(Form("hnsvtx_C%d", i));
		hsvtxm   [i]= (TH2D*) f->Get(Form("hsvtxm_C%d", i));
		hsvtxdl  [i]= (TH2D*) f->Get(Form("hsvtxdl_C%d", i));
		hsvtxdls [i]= (TH2D*) f->Get(Form("hsvtxdls_C%d", i));
		hsvtxntrk[i]= (TH2D*) f->Get(Form("hsvtxntrk_C%d", i));
	}
}

#endif
