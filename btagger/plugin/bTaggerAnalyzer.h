

#ifndef BTAGGERANALYZER_H
#define BTAGGERANALYZER_H

#include "treeScanner.h"
#include "Utility.h"
#include "jtcUti.h"
#include "plotLib.h"

//corresponds to the AASetup eventMaps

class bTaggerAnalyzer: public scanPlugin{
	enum flavorID {udsg, c, b, unknown};
	public : 
	bTaggerAnalyzer(TString name): scanPlugin(), ana_name(name){};
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
	void loadStep1File(TFile *f);

	stackPlot* projFlavor(TH2 *h, bool norm = 0, int rebin =1){
		TString hname = h->GetName();
		auto sh = new stackPlot("stack_"+hname);
		auto hudsg = (TH1D*) h->ProjectionX(hname+"_udsg", flavorID::udsg, flavorID::udsg);
		auto hc = (TH1D*) h->ProjectionX(hname+"_c", flavorID::c, flavorID::c);
		auto hb = (TH1D*) h->ProjectionX(hname+"_b", flavorID::b, flavorID::b);
		auto hall = (TH1D*) h->ProjectionX(hname+"_all", flavorID::udsg, flavorID::b);
		Double_t s = hall->Integral();
		if(norm) {
			hudsg->Scale(1.0/s);
			hc->Scale(1.0/s);
			hb->Scale(1.0/s);
		}
		if(rebin > 1) {
			hudsg->Rebin(rebin);
			hc   ->Rebin(rebin);
			hb   ->Rebin(rebin);
		}
		sh->addLegend();
		sh->add(hudsg, "udsg");
		sh->add(hc, "c jet");
		sh->add(hb, "b jet");
		sh->defaultColor();
		return sh;
	}
	void hist_style(TH1* h, TString xtitle){
			h->GetXaxis()->SetTitle(xtitle);
			h->GetXaxis()->CenterTitle();
	}
	void stackStyle(THStack* h, TString xtitle){
			h->GetXaxis()->SetTitle(xtitle);
	}
	multi_pads *drawStack(TString name,TH2D **h, TString xtitle, int rebin = 1){
		int ncent = cent->nbins;
		auto c = new multi_pads(name, "", 1, ncent);
		for(int i=0; i< ncent; ++i){
			c->CD(0, ncent-i-1);
			auto sh = projFlavor(h[i], 1, rebin);
			sh->Draw("hist");
			stackStyle(sh, xtitle);
			sh->Draw("hist");
			cent->addCentLabel(i);
			if(i == ncent-1) sh->legend->Draw();
		}
		return c;
	}

	bool (*recoJetCut)(eventMap*em, int j) = 0; // return 1 to skip;

	centralityHelper *cent=nullptr;
	TH1D *hvz, *hpthat, *hcent;
	TH2D** pdisc, **ndisc, **disc, **jtpt, **jteta, **jtphi;
	TH2D** hnsvtx, **hsvtxm, **hsvtxdl, **hsvtxdls, **hsvtxntrk;
	TString js_name, ana_name;
};

int bTaggerAnalyzer::allocateHists(){
	//return the # of centrailty bins
	int ncent = cent->nbins;
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
	return ncent;
}

void bTaggerAnalyzer::beginJob(){
	hm = new histManager();
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
		jtpt[i]=hm->regHist<TH2D>(Form("jtpt_C%d", i), "jet pT distribution "+centl, default_setup::nptbin , default_setup::ptbin, 5, 0, 5);
		jteta[i]=hm->regHist<TH2D>(Form("jteta_C%d", i), "jet eta distribution "+centl, 50, -2.5, 2.5, 5, 0, 5);
		jtphi[i]=hm->regHist<TH2D>(Form("jtphi_C%d", i), "jet phi distribution "+centl, 50, -TMath::Pi(), TMath::Pi(), 5, 0, 5);
		pdisc[i]=hm->regHist<TH2D>(Form("pTagger_C%d", i), "positive tagger "+centl, 110, 0, 1.1, 5, -0.5, 4.5);
		ndisc[i]=hm->regHist<TH2D>(Form("nTagger_C%d", i), "negative tagger "+centl, 110, 0, 1.1, 5, -0.5, 4.5);
		disc [i]=hm->regHist<TH2D>(Form("wTagger_C%d", i), "working tagger "+centl, 110, 0, 1.1, 5, -.5, 4.5);
		hnsvtx [i]=hm->regHist<TH2D>(Form("hnsvtx_C%d", i), "# of SV "+centl, 20, 0, 20, 5, -.5, 4.5);
		hsvtxm [i]=hm->regHist<TH2D>(Form("hsvtxm_C%d", i), "SV mass "+centl, 20, 0, 10, 5, -.5, 4.5);
		hsvtxdl [i]=hm->regHist<TH2D>(Form("hsvtxdl_C%d", i), "SV distance "+centl, 60, 0, 3, 5, -0.5,4.5);
		hsvtxdls[i]=hm->regHist<TH2D>(Form("hsvtxdls_C%d", i), "SV distance significance "+centl, 200, 0, 200, 5, -0.5, 4.5);
		hsvtxntrk [i]=hm->regHist<TH2D>(Form("hsvtxntrk_C%d", i), "# of trks assoicated to SV "+centl, 30, 0, 30, 5, -0.5, 4.5);
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
	}
}

void bTaggerAnalyzer::endJob(){
	auto wf = TFile::Open(ana_name+"_step1.root", "recreate");
	hm->write();
	wf->Close();
}

void bTaggerAnalyzer::loadStep1File(TFile *f){
	int ncent = allocateHists();
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
		hist_style((TH1*) jtpt[i] , "p_{T}^{jet}");
		hist_style((TH1*) jteta[i] , "#eta^{jet}");
		hist_style((TH1*) jtphi[i] , "#phi^{jet}");
		hist_style((TH1*) ndisc[i], "negative CSV");
		hist_style((TH1*) pdisc[i], "positive CSV");
		hist_style((TH1*) disc [i], "CSV discriminator ");
		hist_style((TH1*) hnsvtx[i], "# of SV");
	}
}

#endif
