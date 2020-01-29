
#ifndef BTAGGERANALYZER_H
#define BTAGGERANALYZER_H

#include "TH3.h"
#include "myProcesses/jtc/plugin/treeScanner.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/plugin/matrixTH1Ptr.h"
#include "myProcesses/jtc/plugin/xAxis.h"

//corresponds to the AASetup eventMaps
class eventMap;
class bTaggerAnalyzer: public scanPlugin{
	struct probeSet{
		int csvp;
		TH2D **hjetpt, **hjeteta, **hjetphi;
		TH2D **hjec_tag, **hjec_b;
	};
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
	void initM2(matrixTH1Ptr &h , TString name);
	virtual void beginJob();
	int allocateHists();
	virtual void endJob();
	virtual void run();
	void fillProbeSet(int j, int ijet, flavorID flavor, eventMap *em, float weight);
	void loadStep1File(TFile *f, bool isMC);
	void checkJetPtBin(int n, float *bins){
		doPtCSV = 1;
		njetpt = n; jetptbins = bins;
		jtptaxis = new xAxis(n, bins);
	}
	bool (*recoJetCut)(eventMap*em, int j) = 0; // return 1 to skip;
	bool isMC, doPtCSV;
	int ncent, nflavor = int(flavorID::unknown+1);
	Double_t flavorbin[5] = {-.5, .5, 1.5, 2.5, 3.5};
	centralityHelper *cent=nullptr;
	int njetpt;
	float *jetptbins;
	void probeCSV(TString name, float point);

	xAxis *jtptaxis;
	TH1D *hvz, *hpthat, *hcent;
	TH2D** pdisc, **ndisc, **disc, **jtpt, **jteta, **jtphi;
	TH2D** hnsvtx, **hsvtxm, **hsvtxdl, **hsvtxdls, **hsvtxntrk;
	matrixTH1Ptr m2pdisc, m2ndisc, m2disc;
	TString js_name, ana_name;
	std::vector<probeSet> jpset;
};

void bTaggerAnalyzer::probeCSV(TString name, float point){
	jpset.emplace_back(probeSet());
	probeSet &ps = jpset.back();
	ncent = cent->nbins;
	ps.csvp = point;
	ps.hjetpt = new TH2D*[ncent];
	ps.hjeteta= new TH2D*[ncent];
	ps.hjetphi= new TH2D*[ncent];
	ps.hjec_tag = new TH2D*[ncent];
	ps.hjec_b = new TH2D*[ncent];
	for(int i=0; i<njetpt; ++i){
		ps.hjetpt [i] = hm->regHist<TH2D>(name+"/"+name+Form("_jetpt_C%d",i), "",default_setup::nptbin , default_setup::ptbin, 5, -0.5, 4.5);
		ps.hjeteta[i]= hm->regHist<TH2D>(name+"/"+name+Form("_jeteta_C%d",i), "", 50, -2.5, 2.5, 5, -0.5, 4.5);
		ps.hjetphi[i]= hm->regHist<TH2D>(name+"/"+name+Form("_jetphi_C%d",i), "",50, -TMath::Pi(), TMath::Pi(), 5, -0.5, 4.5);
		ps.hjec_tag [i]= hm->regHist<TH2D>   (name+"/"+name+Form("_jec_tag_C%d",i), "",default_setup::nptbin , default_setup::ptbin, 20, 0, 2);
		ps.hjec_b [i]= hm->regHist<TH2D>   (name+"/"+name+Form("_jec_b_C%d",i), "",default_setup::nptbin , default_setup::ptbin, 20, 0, 2);
	}
}

void  bTaggerAnalyzer::initM2(matrixTH1Ptr &m2, TString name){
	m2.setup(name, njetpt,ncent);
	for(int i=0; i<njetpt; ++i){
		for(int j= 0; j<ncent; ++j){
			TString centl  = cent->centLabel[j];
			TString ptl = TString(Form("%d-%d",int(jetptbins[i]), int(jetptbins[i+1])));
			m2.add((TH1*) hm->regHist<TH2D>("csvJetPtBins/"+name+Form("_P%d_C%d",i,j),"jet pT: "+ptl+", "+centl, 110,0, 1.1, 5, -0.5, 4.5), i,j) ;
		}
	}
}

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
		hnsvtx  [i]=hm->regHist<TH2D>(Form("QAs/hnsvtx_C%d", i), "# of SV "+centl, 20, 0, 20, 5, -.5, 4.5);
		hsvtxm  [i]=hm->regHist<TH2D>(Form("QAs/hsvtxm_C%d", i), "SV mass "+centl, 20, 0, 10, 5, -.5, 4.5);
		hsvtxdl [i]=hm->regHist<TH2D>(Form("QAs/hsvtxdl_C%d", i), "SV distance "+centl, 60, 0, 3, 5, -0.5,4.5);
		hsvtxdls[i]=hm->regHist<TH2D>(Form("QAs/hsvtxdls_C%d", i), "SV distance significance "+centl, 200, 0, 200, 5, -0.5, 4.5);
		hsvtxntrk [i]=hm->regHist<TH2D>(Form("QAs/hsvtxntrk_C%d", i), "# of trks assoicated to SV "+centl, 30, 0, 30, 5, -0.5, 4.5);
	}
	if(doPtCSV){
		initM2(m2disc, "m2disc");
		initM2(m2pdisc, "m2pdisc");
		initM2(m2ndisc, "m2ndisc");
	}
};

void bTaggerAnalyzer::run(){
	int jcent = cent->jcent(em->hiBin);
	if(jcent < 0) return;
	float evtW= em->isMC ? em->weight : 1;
	evtW = evtW*ts->evtW;
	hvz->Fill(Double_t(em->vz), evtW);
	hcent->Fill(Double_t(em->hiBin), evtW);
	if(em->isMC) hpthat->Fill(em->pthat, evtW);
	if(em->isMC) if(em->pthat < 60) return;
	for(int i=0; i< em->nJet(); ++i){
		if(recoJetCut(em, i)) continue;
		//if(em->isMC) if(em->ref_jetpt[i] < 50) continue;
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
		if(em->isMC){
			for(auto & it : jpset ){

			}
		}
		fillProbeSet(jcent, i, flavor,em, evtW);
		if(doPtCSV){
			int jpt = jtptaxis->find_bin_in_range(em->jetpt[i]);
			if(jpt < 0) continue;
			((TH2*)(m2disc.at(jpt,jcent)))->Fill(em->disc_csvV2[i],flavor, evtW);
			((TH2*)(m2pdisc.at(jpt,jcent)))->Fill(em->pdisc_csvV2[i],flavor, evtW);
			((TH2*)(m2ndisc.at(jpt,jcent)))->Fill(em->ndisc_csvV2[i],flavor, evtW);
		}
	}
}

void bTaggerAnalyzer::endJob(){
	auto wf = TFile::Open(ana_name, "recreate");
	hm->write(wf);
	wf->Close();
}

void bTaggerAnalyzer::loadStep1File(TFile *f, bool ismc){
	isMC = ismc;
	ncent = allocateHists();
	hvz = (TH1D*) f->Get("hvz");
	hcent = (TH1D*) f->Get("hcent");
	for(int i=0;i<ncent; ++i){
		jtpt     [i]= (TH2D*) f->Get(Form("jtpt_C%d", i));
		jteta    [i]= (TH2D*) f->Get(Form("jteta_C%d", i));
		jtphi    [i]= (TH2D*) f->Get(Form("jtphi_C%d", i));
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

void bTaggerAnalyzer::fillProbeSet(int j, int i, flavorID flavor , eventMap *em, float weight){
	for(auto & ps : jpset){
		if( em->disc_csvV2[i] < ps.csvp) continue;
		ps.hjetpt[j] ->Fill(em->jetpt [i], flavor, weight);
		ps.hjeteta[j]->Fill(em->jeteta[i], flavor, weight);
		ps.hjetphi[j]->Fill(em->jetphi[i], flavor, weight);
		if( em->ref_jetpt[i] < 0) continue;
		ps.hjec_tag[j]->Fill(em->ref_jetpt[i], em->jetpt[i]/em->ref_jetpt[i], weight);
		if(flavor != flavorID::b) continue;
		ps.hjec_b[j]->Fill(em->ref_jetpt[i], em->jetpt[i]/em->ref_jetpt[i], weight);
	}
}

#endif
