
#include "forestDataset.h"
#include "rootScanner.h"
#include "histManager.h"
#include "utility.h"

class trkAnalyzer: public rootAnalyzer {
	public :
		bool isMC = 0;
		ppTrack *trk;
		hiJet *jet;
		evtTree *evtT;
		hltTree *hlt;
		TString jetSet;
		histManager *hm;
		TH1F* hvz, *hpthat;
		TH2F *heta_algo, *hphi_algo, *hpt_algo;
		virtual int beginJob();
		virtual int endJob();
		virtual int analyze();
		trkAnalyzer();
};

trkAnalyzer::trkAnalyzer(){
}

int trkAnalyzer::beginJob(){
	TTree* t_trk = handle("ppTrack/trackTree");
	TTree* t_jet = handle(jetSet+"/t");
	TTree* t_hlt = handle("skimanalysis/HltTree");
	TTree* t_evt = handle("hiEvtAnalyzer/HiTree");
	trk = new ppTrack(t_trk);
	jet = new hiJet(t_jet);
	evtT = new evtTree(t_evt, isMC);
	hlt= new hltTree(t_hlt);
//booking histogram
	hm = new histManager();

	hvz = hm->regHist<TH1F>("hvz", "vz distribution", 40, -20, 20);
	if(isMC){
		hpthat = hm->regHist<TH1F>("hpthat", "pThat distribution", 50, 0, 300);
	}
	heta_algo = hm->regHist<TH2F>("heta_algo", "trk eta-algo distribution", 46, 0, 46, 23, utility::etabin);
	hphi_algo = hm->regHist<TH2F>("hphi_algo", "trk phi-algo distribution", 46, 0, 46, 17, utility::phibin);
	hpt_algo = hm->regHist<TH2F>("hpt_algo", "trk pt-algo distribution", 46, 0, 46, 8, utility::trkptbin);
	hm->sumw2();
	return 0;
}

int trkAnalyzer::analyze(){
	if( evtT->Cut(0) < 0 ) return 1;
	if( hlt->Cut(0) < 0 ) return 1;
	float weight = 1;
	if( isMC){
		weight = evtT->weight;
		hpthat->Fill(evtT->pthat, weight);
	}
	hvz->Fill(evtT->vz, weight);
	for(int ijet =0; ijet< jet->nref; ijet++){
		if( jet->Cut(ijet) < 0 ) continue;
		for(int itrk =0; itrk< trk->nTrk; itrk++){
			if( trk->Cut(itrk) < 0 ) continue;
			//std::cout<<trk->trkPt[itrk]<<"; "<<trk->trkEta[itrk]<<std::endl;
			float dr = utility::findDr(trk->trkEta[itrk], trk->trkPhi[itrk], jet->jteta[ijet], jet->jtphi[ijet]);
			if( dr> 0.4) continue;
			heta_algo->Fill(trk->trkAlgo[itrk], jet->jteta[ijet]- trk->trkEta[itrk],weight);
			hphi_algo->Fill(trk->trkAlgo[itrk], jet->jtphi[ijet]- trk->trkPhi[itrk],weight);
			hpt_algo->Fill(trk->trkAlgo[itrk], trk->trkPt[itrk], weight);
		}
	}
	return 0;
}

int trkAnalyzer::endJob(){
	auto wf = TFile::Open("scanRes.root", "recreate");
	wf->cd();
	hm->write();
	wf->Close();
	return 0;
}
