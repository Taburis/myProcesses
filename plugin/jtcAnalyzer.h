
#include "histManager.h"
#include "xAxis.h"
#include "eventMap_hiForest.h"
#include "jtcUti.h"
#include <cstdarg>

enum jetType {inclJet, trueBJet};
enum trkType {inclTrk};

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
		jtcAnalyzer(eventMap *em0){ 
			em = em0; isMC=em->isMC;
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
		virtual std::vector<candidate>* genJtSelections( eventMap *em){
			// if the jets have n types, need to add n times...
			std::vector<candidate>* cands = new std::vector<candidate>();
			for(int i=0; i< em->nGenJet(); ++i){
				if(genJtCuts(em, i)) continue;
				//add incljets
				float weight = isMC ? jetPtWeight(em->genjetpt[i]) : 1;
				int flag = jetType::inclJet;
				candidate cc(flag, em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
				cands->push_back(cc);
				//-------------

				//add true b jets
				int index = em->genMatchIndex[i];
				if(index > -1) if(TMath::Abs(em->flavor_forb[index]) == 5) 
					flag = jetType::trueBJet;
				candidate cc2(flag, em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
				cands->push_back(cc2);
				//-------------
			}
			return cands;
		}
		void quickHistReg(TString cap, TString dsname,  histManager *h, histCase &hc, int nPt, int nCent){
			int nHistoBinsX = 500;
			int nHistoBinsY = 200;
			hc.sig= new TH2D*[nPt*nCent];
			hc.sig_pTweighted= new TH2D*[nPt*nCent];
			hc.mixing= new TH2D*[nPt*nCent];
			TString tmp, name;
			name = cap+dsname;
			for(int i=0; i<nPt; ++i){
				for(int j=0; j<nCent; ++j){
					tmp = centLabel[j]+"_"+centLabel[j+1]+"_"+ptLabel[i]+"_"+ptLabel[i+1];
					hc.sig[i+j*nPt] = hm->regHist<TH2D>(name+Form("_%d_%d",i, j), tmp,
							nHistoBinsX,-5,5,nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
					hc.sig_pTweighted[i+j*nPt] = hm->regHist<TH2D>(name+Form("_pTweighted_%d_%d",i, j), tmp,
							nHistoBinsX,-5,5, nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
					hc.mixing[i+j*nPt] = hm->regHist<TH2D>(name+Form("_mixing_%d_%d",i, j), tmp,
							nHistoBinsX,-5,5, nHistoBinsY,-TMath::Pi()/2,3*TMath::Pi()/2);
				}
			}

			const float newbin [21] = {110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360,
				380, 400, 432, 500};
			int nbin = 20;
			name=cap;
			hc.jet_pt = new TH1D*[nCent];
			hc.jet_eta = new TH1D*[nCent];
			hc.jet_phi = new TH1D*[nCent];
			const float wta_ptbin [17] = {1, 2, 3, 4,5 ,6,7 ,8, 10, 12, 14, 16, 18, 20, 50, 100, 500};
			for(int j=0; j<nCent; ++j){
				tmp = centLabel[j]+" to "+centLabel[j+1];
				hc.jet_pt[j] = hm->regHist<TH1D>(name+Form("_corrpt_%d",j), tmp, nbin, newbin);
				hc.jet_eta[j] = hm->regHist<TH1D>(name+Form("_eta_%d",j), tmp, 100, -2.0, 2.0);
				hc.jet_phi[j] = hm->regHist<TH1D>(name+Form("_phi_%d",j), tmp, 72, -TMath::Pi(), TMath::Pi());
			}
		}
		virtual void produce(histCase &hc, int centj, std::vector<candidate>*jetCand, jetType flag1, std::vector<candidate> *trkCand, trkType flag2, float evtWeight, bool fillMix = 0){
			for(auto i = 0;i<jetCand->size(); i++){
				if(jetCand->at(i).flag != flag1) continue;
				hc.jet_pt[centj]->Fill(jetCand->at(i).pt, evtWeight*(jetCand->at(i).weight));
				hc.jet_eta[centj]->Fill(jetCand->at(i).eta, evtWeight*(jetCand->at(i).weight));
				hc.jet_phi[centj]->Fill(jetCand->at(i).phi, evtWeight*(jetCand->at(i).weight));
				for(auto j = 0;j<trkCand->size(); j++){
					if(trkCand->at(j).flag != flag2) continue;

					int ptj = ptax->findBin(safeValue(trkCand->at(j).pt,ptbins[nPt+1]));
					float dphic=(jetCand->at(i)).phi-(trkCand->at(i)).phi;

					while(dphic>(1.5*TMath::Pi())){dphic+= -2*TMath::Pi();}
					while(dphic<(-0.5*TMath::Pi())){dphic+= 2*TMath::Pi();}

					float weightc = evtWeight*(jetCand->at(i).weight)*(trkCand->at(j).weight);
					float detac = jetCand->at(i).eta-trkCand->at(j).eta;
					if(fillMix){
						hc.mixing[ptj+nPt*centj]->Fill(detac, dphic, weightc);
					}else {
						hc.sig[ptj+nPt*centj]->Fill(detac, dphic, weightc);
						hc.sig_pTweighted[ptj+nPt*centj]->Fill(detac, dphic, trkCand->at(j).pt*weightc);
					}
				}
			}
		}
		float safeValue(float in, float max){
			//to prevent the overflow.
			if(in> max) return max -1;
			else return in;
		}
		void init(){
			hm = new histManager();
			ptax = new xAxis(nPt, ptbins);
			centax= new xAxis(nCent, centbins);
			hvz = hm->regHist<TH1D>("vzInfo", "", 200, -20, 20);
			hcent = hm->regHist<TH1D>("centInfo","",  50, 0, 200);
			if(isMC) hpthat = hm->regHist<TH1D>("pthatInfo", "", 100, 0, 400);
			quickHistReg("inclJet_", "GenJet_GenTrk", hm, inclCase, nPt, nCent);
			quickHistReg("trueBJet_", "GenJet_GenTrk", hm, trueBCase, nPt, nCent);
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
		histManager *hm;
		int nPt=6, nCent= 1;
		Long64_t nevt = -1; 
		xAxis *ptax , *centax; 
		TH1D* hvz, *hcent, *hpthat;

		float *ptbins = 0;
		float *centbins = 0;
		TString *ptLabel = 0;
		TString *centLabel = 0;
		histCase inclCase, trueBCase;
};

void jtcAnalyzer::loop(){
	init();
	histCase hc;
	Long64_t nentries = nevt < 0 ? em->evtTree->GetEntriesFast() : nevt;
	int centj = 0;
	for(Long64_t jentry = 0; jentry< nentries; ++jentry){
		if(jentry%1000 ==0 ) std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
		em->getEvent(jentry);	
		if(em->checkEventFilter()) continue;	
		float evtWeight = 1;
		auto gj = genJtSelections( em);
		auto gp = genParticleSelections( em);
		produce(trueBCase, centj, gj, jetType::trueBJet, gp, trkType::inclTrk, evtWeight);
	}
	write("correlation.root");
}


