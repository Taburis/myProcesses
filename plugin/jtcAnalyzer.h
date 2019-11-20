
#include "histManager.h"
#include "xAxis.h"
#include "eventMap_hiForest.h"
#include "jtcorrelator.h"
#include "jtcUti.h"

struct histCase{
	//if want to add any hist, need to add it in the quickHistReg as well, and add the filling in the fillCase
	TH2D** sig;
	TH2D** sig_pTweighted;
	TH2D** mixing;
	TH1D** jet_corrpt;
	TH1D** jet_eta;
	TH1D** jet_phi;
	TH1D** jt_wta_eta;
	TH1D** jt_wta_phi;
	TH1D** jt_wta_deta;
	TH1D** jt_wta_dphi;
};



class jtcAnalyzer{
	public : 
		jtcAnalyzer(eventMap *em0, int npt, int ncent){ 
			em = em0; isMC=em->isMC;
			genJetCand = new candidates(); genTrkCand = new candidates(); mixGenTrkCand = new candidates();
}
		~jtcAnalyzer(){}
		virtual bool recoTrkCuts(eventMap *em, int j){
			//return 1 to skip
			if(em->trkpt[j] < 1 || em->trkpt[j]>400) return 1;
			if(TMath::Abs(em->trketa[j]) >= 2.4) return 1;
			if(!(em->highPurity[j])) return 1;
			if(em->trknhit[j]< 11) return 1;
			if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > 0.15) return 1;
			float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
			if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j]))) return 1;
			return 0;	
		}
		virtual bool recoJtCuts(eventMap *em, int j){
			//return 1 to skip
			if(em->jetpt[j] < 120.0) return 1;
			if(TMath::Abs(em->jeteta[j]) > 1.6) return 1;
			return 0;
		}
		virtual bool genParticleCuts(eventMap *em, int j){
			//return 1 to skip
			if(em->gppt(j) < 1.0) return 1;
			if(TMath::Abs(em->gpeta(j)) > 2.4) return 1;
			if(em->gpchg(j)==0) return 1;
			return 0;
		}
		virtual bool genJtCuts(eventMap * em, int j){
			//return 1 to skip
			if( em->genjetpt[j] < 120 ) return 1;
			if( TMath::Abs(em->genjet_wta_eta[j]) > 1.6 ) return 1;
			return 0;
		}
		virtual void recoTrkSelections(candidates *jc, eventMap *em){
			for(int i=0; i< em->nTrk(); ++i){
				if(recoTrkCuts(em, i)) continue;
				jc->add(em->trkpt[i], em->trketa[i], em->trkphi[i], 1);	
			}
		}
		virtual void genParticleSelections(candidates *jc, eventMap *em){
			for(int i=0; i< em->nGP(); ++i){
				if(genParticleCuts(em, i)) continue;
				jc->add(em->gppt(i), em->gpeta(i), em->gpphi(i), 1);	
			}
		}
		// flag for jets : 1. inclusive 2: tagged 3: tagged & b 4: true b
		virtual void recoJtSelections(candidates *jc, eventMap *em){
			for(int i=0; i< em->nJet(); ++i){
				if(recoJtCuts(em, i)) continue;
				int flag = 1;
				float weight = isMC ? jetPtWeight(em->jetpt[i]) : 1;
				jc->add(em->jetpt[i], em->jeteta[i], em->jetphi[i], 1, weight );
			}
		}
		virtual void genJtSelections(candidates *jc, eventMap *em){
			for(int i=0; i< em->nGenJet(); ++i){
				if(genJtCuts(em, i)) continue;
				int flag = 1;
				int index = em->genMatchIndex[i];
				if(index > -1) if(TMath::Abs(em->flavor_forb[index]) == 5) flag = 4;
				float weight = isMC ? jetPtWeight(em->genjetpt[i]) : 1;
				jc->add(em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], 1, weight);
			}
		}

		float (*jetPtWeight)(float pt) = 0; //return 1 to skip
		void loop();
		bool isMC;
		eventMap * em, *mixem;
		candidates *genTrkCand, *genJetCand, *mixGenTrkCand;
		jtcorrelator jc; 
		histCase inclCase, trueBCase;
};

void jtcAnalyzer::loop(){
	Long64_t nentries = em->evtTree->GetEntriesFast();
	int truebFlag [] = {1, 4};
	int inclTrkFlag [] = {1};
	for(Long64_t jentry = 0; jentry< nentries; ++jentry){
		if(jentry%1000 ==0 ) std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
		em->getEvent(jentry);
		if(em->checkEventFilter()) continue;
//		cout<<"done0"<<endl;
		genJtSelections(genJetCand, em);
//		cout<<"done1"<<endl;
		genParticleSelections(genTrkCand, em);
//		cout<<"done2"<<endl;
		jc.addJt(genJetCand);
		jc.addTrk(genTrkCand);
//		cout<<"done3"<<endl;
		jc.produce<2,1>(truebFlag, inclTrkFlag);
		jc.clear();
//		cout<<"done"<<endl;
	}
}
