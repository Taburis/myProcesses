
#include "histManager.h"
#include "xAxis.h"
#include "eventMap_hiForest.h"
//#include "jtcorrelator.h"
#include "jtcUti.h"
#include <cstdarg>

class candidate {
        public : 
                candidate(){};
                candidate(int flag0, float pt0,float eta0,float phi0, float w= 1){ 
			set(flag0, pt0,eta0,phi0,w);
		}
// this will raise unknown error
//		candidate(candidate &c ){
//                        this->copy(c);
//                }
                void copy(candidate &c){
                        pt=c.pt;eta=c.eta;phi=c.phi;weight=c.weight;flag=c.flag;isValid = c.isValid;
                }
                candidate & operator = (candidate &rhs){
                        this->copy(rhs);
                        return (*this);
                }
                ~candidate(){};
                void set(int flag0, float pt0,float eta0,float phi0, float w= 1){
                        pt=pt0; 
                        eta=eta0;
                        phi=phi0;
                        flag= flag0;
                        weight=w;
			isValid = 1;
                }
                void clear(){isValid = 0;}

		bool flagMask(int nflgs, ...){
			// return 1 for the flags not listed above (to skip)
			// the 1st argument is the length of flag argument following.
			if(nflgs ==0)return 0;
			va_list args;
			va_start(args, nflgs);
			for(int i=0;i<nflgs; i++){
				int fg = va_arg(args, int);
				if (flag == fg) return 0;
			}
			return 1;
		}
		float pt, eta, phi, weight;
		int flag;
		bool isValid= 0;
};

class jtcAnalyzer{
	public : 
		jtcAnalyzer(eventMap *em0, int npt, int ncent){ 
			em = em0; isMC=em->isMC;
	//		genJetCand = new candidates(); genTrkCand = new candidates(); mixGenTrkCand = new candidates();
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
//		virtual void recoTrkSelections(candidates *jc, eventMap *em){
//			for(int i=0; i< em->nTrk(); ++i){
//				if(recoTrkCuts(em, i)) continue;
//				jc->add(em->trkpt[i], em->trketa[i], em->trkphi[i], 1);	
//			}
//		}
		virtual std::vector<candidate>* genParticleSelections( eventMap *em){
			std::vector<candidate>* cands = new std::vector<candidate>();
			for(int i=0; i< em->nGP(); ++i){
				if(genParticleCuts(em, i)) continue;
				candidate cc(em->gppt(i),em->gpeta(i),em->gpphi(i),1);
				cands->push_back(cc);
			}
			return cands;
		}
//		virtual void recoJtSelections(candidates *jc, eventMap *em){
//			for(int i=0; i< em->nJet(); ++i){
//				if(recoJtCuts(em, i)) continue;
//				int flag = 1;
//				float weight = isMC ? jetPtWeight(em->jetpt[i]) : 1;
//				jc->add(em->jetpt[i], em->jeteta[i], em->jetphi[i], 1, weight );
//			}
//		}
//		virtual void genJtSelections(candidates *jc, eventMap *em){
//			for(int i=0; i< em->nGenJet(); ++i){
//				if(genJtCuts(em, i)) continue;
//				int flag = 1;
//				int index = em->genMatchIndex[i];
//				if(index > -1) if(TMath::Abs(em->flavor_forb[index]) == 5) flag = 4;
//				float weight = isMC ? jetPtWeight(em->genjetpt[i]) : 1;
//				jc->add(em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], 1, weight);
//			}
//		}
		void run(){
			hm = new histManager();
			TString cap = type2name(recoType::trueBJet)+"_"+makeRecoTypeName(recoType::genjet, recoType::gentrk);
			//quickHistReg(hm, cap, trueBCase, nPtBin, nCentBin);
			loop();
			write("correlation.root");
		}
		void write(std::string name){
			auto wf = TFile::Open(name.c_str(), "recreate");
			wf->cd();
			hm->write();
			wf->Close();
		}

		float (*jetPtWeight)(float pt) = 0; //return 1 to skip
		void loop();
		bool isMC;
		eventMap * em, *mixem;
		//candidates *genTrkCand, *genJetCand, *mixGenTrkCand;
		//jtcorrelator jc; 
		histManager *hm;
		int nPtBin =6, nCentBin = 1;
		Long64_t nevt = -1; 

		histCase inclCase, trueBCase;
};

void jtcAnalyzer::loop(){
	Long64_t nentries = nevt < 0 ? em->evtTree->GetEntriesFast() : nevt;
	int truebFlag [] = {4};
	int inclTrkFlag [] = {1};
	for(Long64_t jentry = 0; jentry< nentries; ++jentry){
		if(jentry%1000 ==0 ) std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
		em->getEvent(jentry);	
		if(em->checkEventFilter()) continue;
		float evtWeight = 1;
//		genJtSelections(genJetCand, em);
		if(gp->size() > 1) nn = gp->at(0).pt;
		cout<<nn<<endl;
//		jc.evtWeight = evtWeight;
//		jc.addJt(genJetCand);
//		jc.addTrk(genTrkCand);
//		jc.produce<1,1>(truebFlag, inclTrkFlag, trueBCase);
//		jc.clear();
	}
}


