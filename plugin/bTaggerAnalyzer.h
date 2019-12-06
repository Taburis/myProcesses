

#ifndef BTAGGERANALYZER_H
#define BTAGGERANALYZER_H

#include "treeScanner.h"
#include "Utility.h"
#include "jtcUti.h"
#include "plotLib.h"

//corresponds to the AASetup eventMaps

class bTaggerAnalyzer: public scanPlugin{
		enum flavorID {udsg, c, b, unknown};
		struct histBundle {TString name; TH1 *hudsg=nullptr, *hc=nullptr, *hb=nullptr, *hdata=nullptr;};
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
		void loadStep1File(TFile *f, bool isMC);

		void projFlavor(histBundle &hb, TH2* h, bool isData){
				hb.name = h->GetName();
				if(isData){
						hb.hdata = (TH1*)h->ProjectionX(hb.name+"_data", flavorID::unknown+1, flavorID::unknown+1);
				}else {
						hb.hudsg = (TH1*) h->ProjectionX(hb.name+"_udsg", flavorID::udsg+1, flavorID::udsg+1);
						hb.hc = (TH1*) h->ProjectionX(hb.name+"_c", flavorID::c+1, flavorID::c+1);
						hb.hb = (TH1*) h->ProjectionX(hb.name+"_b", flavorID::b+1, flavorID::b+1);
				}
				return hb;
		}

		stackPlot* addHistBundle(histBundle &hb){
				auto sh = new stackPlot("stack_"+hb.name);
				sh->addLegend();
				sh->add(hb.hudsg, "udsg");
				sh->add(hb.hc, "c jet");
				sh->add(hb.hb, "b jet");
				sh->defaultColor();
				sh->addTopHist(hb.hdata, "data");
				return sh;
		}

		void rebinBundle(histBundle &hb, int n){	
				if(hb.hudsg) hb.hudsg->Rebin(n);
				if(hb.hc)    hb.hc   ->Rebin(n);
				if(hb.hb)    hb.hb   ->Rebin(n);
				if(hb.hdata!=nullptr) hb.hdata->Rebin(n);
		}

		void topHist_style(TH1* h){
				h->SetMarkerStyle(20);
				h->SetMarkerSize(0.7);
				h->SetMarkerColor(kBlack);
		}

		void stackStyle(THStack* h, TString xtitle){
				h->GetXaxis()->SetTitle(xtitle);
		}

		multi_pads *drawQA(TString name,histBundle **hb, TString xtitle){
				auto c = new multi_pads(name, "", 1, ncent);
				for(int i=0; i< ncent; ++i){
						c->CD(0, ncent-i-1);
						topHist_style(hb[i]->hdata);
						auto sh = addHistBundle(*(hb[i]));
						sh->normalizeStack(0, 1);
						sh->Draw("hist");
						stackStyle(sh, xtitle);
						sh->Draw("hist");
						sh->normalizeOverlayHists(0, 1);
						sh->drawHists();
						cent->addCentLabel(i);
						if(i == ncent-1) sh->legend->Draw();
				}
				return c;
		}

		multi_pads *drawQA(TString name, TH2D** hmc, TH2D** hda, TString xtitle, int rebin=1){
				histBundle ** hb = new histBundle*[ncent];
				for(int i=0; i<ncent; i++){
						hb[i] = new histBundle();
						projFlavor(*(hb[i]), hmc[i],0);
						projFlavor(*(hb[i]), hda[i],1);
						rebinBundle(*(hb[i]), rebin);
				}
				return drawQA(name, hb, xtitle);
		}

		bool (*recoJetCut)(eventMap*em, int j) = 0; // return 1 to skip;

		bool isMC;
		int ncent;
		centralityHelper *cent=nullptr;
		TH1D *hvz, *hpthat, *hcent;
		TH2D** pdisc, **ndisc, **disc, **jtpt, **jteta, **jtphi;
		TH2D** hnsvtx, **hsvtxm, **hsvtxdl, **hsvtxdls, **hsvtxntrk;
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
