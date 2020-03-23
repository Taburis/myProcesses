
#ifndef BTAGGERANALYZER_H
#define BTAGGERANALYZER_H

#include "TH3.h"
#include "TH1.h"
#include "TProfile.h"
#include "TMath.h"
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
	virtual void beginJob();
	int allocateHists();
	virtual void endJob();
	virtual void run();
	float (*jtWeight)(eventMap*em, int, int);
	void fillProbeSet(int j, int ijet, flavorID flavor, eventMap *em, float weight);
	void loadStep1File(TFile *f, bool isMC);
	bool (*recoJetCut)(eventMap*em, int j) = 0; // return 1 to skip;
	bool isMC;
	float (*jet_weight)(eventMap *em, int ijet)=0;
	int ncent, nflavor = int(flavorID::unknown+1);
	Double_t flavorbin[5] = {-.5, .5, 1.5, 2.5, 3.5};
	centralityHelper *cent=nullptr;

	// the variables for the SF calculation;
	void configSF(int n, double *bins, int ncsv, double *csvb){
		doSF = 1;
		njetptbin_SF = n; jetptbins_SF = bins;
		ncsvbin_SF = ncsv; csvbins_SF = csvb;
	}
	bool doSF = 0;
	int njetptbin_SF, ncsvbin_SF;
	double *jetptbins_SF, *csvbins_SF;
	matrixTH1Ptr m2ndisc, m2wdisc, m2stat;
	void initSFHist();
	void record_SF(eventMap *em, int jjet, int jcent, flavorID flavor, float evtW = 1);
	//--------------------------------

	void probeCSV(TString name, float point);

	TH1D *hvz, *hpthat, *hcent;
	TH2D** pdisc, **ndisc, **disc, **jtpt, **jteta, **jtphi;
	TH2D** hnsvtx, **hsvtxm, **hsvtxdl, **hsvtxdls, **hsvtxntrk;
	TH2D **hcsv_trkMul, **hcsv_trkMomentum, **hcsv_trkPtRel, **hcsv_trk3dIP, **hcsv_trk3dIPSig, **hcsv_trkDist, **hcsv_trkDr;
	TH2D** jec, **jer;
	TString js_name, ana_name;
	std::vector<probeSet> jpset;
};

void bTaggerAnalyzer::initSFHist(){
	m2ndisc.setup("scaleFactor/m2negativeTagger", ncsvbin_SF, ncent);
	m2wdisc.setup("scaleFactor/m2workingTagger", ncsvbin_SF, ncent);
	m2stat.setup("scaleFactor/m2stat", 1, ncent);
	std::stringstream sstream;
	for(int i=0; i<ncsvbin_SF; ++i){
		for(int j=0; j<ncent; ++j){
			TString centl  = cent->centLabel[j];
			sstream.str(std::string());
			sstream<<"negative CSV > "<<float(csvbins_SF[i]);
			TString name = "scaleFactor/m2negativeTagger";
			m2ndisc.add((TH1*)hm->regHist<TH2D>(name+Form("_CSV%d_C%d",i,j),sstream.str()+", "+centl, njetptbin_SF, jetptbins_SF, 5, -0.5, 4.5), i,j);
			sstream.str(std::string());
			name = "scaleFactor/m2workingTagger";
			sstream<<"working CSV > "<<float(csvbins_SF[i]);
			m2wdisc.add((TH1*)hm->regHist<TH2D>(name+Form("_CSV%d_C%d",i,j),sstream.str()+", "+centl, njetptbin_SF, jetptbins_SF, 5, -0.5, 4.5),i,j);
		}
	}
	for(int j=0; j<ncent; ++j){
		TString name = "scaleFactor/m2Stat";
		TString centl  = cent->centLabel[j];
		m2stat.add((TH1*)hm->regHist<TH2D>(name+Form("_S0_C%d",j),"jets:"+centl, njetptbin_SF, jetptbins_SF, 5, -0.5, 4.5), 0,j);
	}
}

void bTaggerAnalyzer::record_SF(eventMap *em, int j, int jcent, flavorID flavor, float evtW){
	float csv = em->disc_csvV2[j], ncsv = em->ndisc_csvV2[j];

	((TH2*)m2stat.at(0,jcent))->Fill(em->jetpt[j], flavor, evtW);
	for(auto i=0; i< ncsvbin_SF; ++i){
		if(csv >=csvbins_SF[i]) 
			((TH2*)m2wdisc.at(i,jcent))->Fill(em->jetpt[j], flavor, evtW);
		if(ncsv>=csvbins_SF[i])
			((TH2*)m2ndisc.at(i,jcent))->Fill(em->jetpt[j], flavor, evtW);
	}
}

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
	for(int i=0; i<ncent; ++i){
		ps.hjetpt [i] = hm->regHist<TH2D>(name+"/"+name+Form("_jetpt_C%d",i), "",default_setup::nptbin , default_setup::ptbin, 5, -0.5, 4.5);
		ps.hjeteta[i]= hm->regHist<TH2D>(name+"/"+name+Form("_jeteta_C%d",i), "", 50, -2.5, 2.5, 5, -0.5, 4.5);
		ps.hjetphi[i]= hm->regHist<TH2D>(name+"/"+name+Form("_jetphi_C%d",i), "",50, -TMath::Pi(), TMath::Pi(), 5, -0.5, 4.5);
		ps.hjec_tag [i]= hm->regHist<TH2D> (name+"/"+name+Form("_jec_tag_C%d",i), "",default_setup::nptbin , default_setup::ptbin, 20, 0, 2);
		ps.hjec_b [i]= hm->regHist<TH2D>   (name+"/"+name+Form("_jec_b_C%d",i), "",default_setup::nptbin , default_setup::ptbin, 20, 0, 2);
	}
}


int bTaggerAnalyzer::allocateHists(){
	//return the # of centrailty bins
	ncent = cent->nbins;
	jec = new TH2D*[ncent];
	jer = new TH2D*[ncent];
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
	hcsv_trkMomentum = new TH2D*[ncent];
	hcsv_trk3dIP = new TH2D*[ncent];
	hcsv_trk3dIPSig = new TH2D*[ncent];
	hcsv_trkDist = new TH2D*[ncent];
	hcsv_trkDr = new TH2D*[ncent];
	hcsv_trkPtRel = new TH2D*[ncent];
	hcsv_trkMul = new TH2D*[ncent];
	return ncent;
}

void bTaggerAnalyzer::beginJob(){
	em->loadJet(js_name);
	em->loadBTagger();
	em->loadBTaggerInputVariables();
	// prepare the pointers for hists
	int ncent = allocateHists();
	hvz = hm->regHist<TH1D>("hvz", "", 150, -15, 15);
	hcent = hm->regHist<TH1D>("hcent", "", 200, 0, 200);
	if(em->isMC){
		hpthat = hm->regHist<TH1D>("hpthat", "", 100, 0, 400);
	}
	for(int i=0; i<ncent; ++i){
		TString centl  = cent->centLabel[i];
		jec[i]=hm->regHist<TH2D>(Form("jec_C%d", i), "JEC profile "+centl, default_setup::nptbin , default_setup::ptbin, 20, 0, 2);
		jer[i]=hm->regHist<TH2D>(Form("jer_C%d", i), "JER profile "+centl, 50, -25, 25, 20, 0, 2);
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
		hcsv_trkMul[i]=hm->regHist<TH2D>(Form("QAs/htrkMul_C%d", i), "jet trk multiplicity "+centl, 30, 0, 30, 5, -0.5, 4.5);
		hcsv_trkMomentum[i]=hm->regHist<TH2D>(Form("QAs/htrkMomentum_C%d", i), "trk(ass. jet) momentum "+centl, 50, 0, 20, 5, -0.5, 4.5);
		hcsv_trkPtRel[i]=hm->regHist<TH2D>(Form("QAs/htrkPtRel_C%d", i), "trk(ass. jet) relative pT "+centl, 50, 0, 15, 5, -0.5, 4.5);
		hcsv_trk3dIP[i]=hm->regHist<TH2D>(Form("QAs/htrk3dIP_C%d", i), "trk(ass. jet) 3D impact paramter "+centl, 55, -0.11, 0.11, 5, -0.5, 4.5);
		hcsv_trk3dIPSig[i]=hm->regHist<TH2D>(Form("QAs/htrk3dIPSig_C%d", i), "trk(ass. jet) 3D impact paramter significance"+centl, 40, 0, 80, 5, -0.5, 4.5);
		hcsv_trkDist[i]=hm->regHist<TH2D>(Form("QAs/htrkDist_C%d", i), "trk(ass. jet) distance from jet axis "+centl, 20, 0, 1, 5, -0.5, 4.5);
		hcsv_trkDr[i]=hm->regHist<TH2D>(Form("QAs/htrkDr_C%d", i), "trk(ass. jet) distance from jet axis "+centl, 20, 0, 1, 5, -0.5, 4.5);
	}
	if(doSF) initSFHist();
	hm->sumw2();
};

void bTaggerAnalyzer::run(){
	int jcent = cent->jcent(em->hiBin);
	if(jcent < 0) return;
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
		if(em->isMC){ float wjt = jtWeight(em, i, jcent);
			evtW= evtW*wjt;
		}

		if(em->ref_jetpt[i] > 0){
			jec [jcent]->Fill(em->jetpt[i], em->jetpt[i]/em->ref_jetpt[i], evtW);	
			jer [jcent]->Fill(em->jetpt[i], em->jetpt[i]-em->ref_jetpt[i], evtW);	
		}
		jtpt [jcent]->Fill(em->jetpt[i],flavor, evtW);	
		jteta[jcent]->Fill(em->jeteta[i],flavor, evtW);	
		jtphi[jcent]->Fill(em->jetphi[i],flavor, evtW);	
		pdisc[jcent]->Fill(em->pdisc_csvV2[i],flavor, evtW);	
		ndisc[jcent]->Fill(em->ndisc_csvV2[i],flavor, evtW);	
		disc[jcent]->Fill(em->disc_csvV2[i],flavor, evtW);	
		hnsvtx[jcent]->Fill(em->nsvtx[i],flavor, evtW);
		hsvtxntrk[jcent]->Fill(em->svtxntrk[i],flavor, evtW);
		hsvtxm	 [jcent]->Fill(em->svtxm   [i],flavor, evtW);
		hsvtxdl  [jcent]->Fill(em->svtxdl  [i],flavor, evtW);
		hsvtxdls [jcent]->Fill(em->svtxdls [i],flavor, evtW);

		hcsv_trkMul[jcent]->Fill(em->csv_trkMul[i],flavor, evtW);
		for(int j= int(em->csv_trkIndexStart[i]); j <int( em->csv_trkIndexEnd[i]); ++j){
//cout<<em->csv_trkIndexStart[i]<<", "<<em->csv_trkIndexEnd[i]<<": "<<j<<endl;
			hcsv_trkMomentum[jcent]->Fill(em->csv_trkMomentum[j],flavor, evtW);
			hcsv_trkPtRel[jcent]->Fill(em->csv_trkPtRel[j],flavor, evtW);
			hcsv_trk3dIP[jcent]->Fill(em->csv_trk3dIP[j],flavor, evtW);
			hcsv_trk3dIPSig[jcent]->Fill(em->csv_trk3dIPSig[j],flavor, evtW);
			hcsv_trkDist[jcent]->Fill(em->csv_trkDist[j],flavor, evtW);
			hcsv_trkDr[jcent]->Fill(em->csv_trkDr[j],flavor, evtW);
		}

		if(em->isMC){
			for(auto & it : jpset ){
			}
		}
		if(doSF) record_SF(em, i, jcent, flavor, evtW);
		fillProbeSet(jcent, i, flavor,em, evtW);
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
