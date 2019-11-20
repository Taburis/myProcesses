
#ifndef CORRELATOR_H
#define CORRELATOR_H
#include "jtcUti.h"

class candidates {
	public : 
		candidates(){};
		~candidates(){};
		void add(float pt0,float eta0,float phi0,float flag0, float w= 1){
			pt.push_back(pt0); 
			eta.push_back(eta0);
			phi.push_back(phi0);
			flag.push_back(flag0);
			weight.push_back(w);
		}
		void clear(){
			pt.clear(); eta.clear(); phi.clear(); weight.clear(); flag.clear(); 
		}
		std::vector<float> pt, eta, phi, weight;
		std::vector<int> flag;
};

class jtcorrelator {
	public : 
		jtcorrelator(){}
		~jtcorrelator(){}
		void addJt(candidates *arr) {jetCand = arr;}
		void addTrk(candidates *arr){trkCand = arr;}
		void clearCorrelationPart(){
			deta.clear(); dphi.clear(), weight.clear();
		}
		void clear(){
			jetCand->clear(); trkCand->clear();
			clearCorrelationPart();
		}
		template< size_t n1, size_t n2>
			void produce(int flag1[n1], int flag2[n2]){
				deta.clear(); dphi.clear(); weight.clear();
				for(auto i = 0;i<jetCand->eta.size(); i++){
					if(n1!=0) 
						if(std::find(flag1, flag1+n1, jetCand->flag[i]) == flag1+n1) 
							continue;
					for(auto j = 0;j<trkCand->eta.size(); j++){
						if(n2!=0) 
							if(std::find(flag2, flag2+n2, trkCand->flag[j]) == flag2+n2) 
								continue;
						float dphic = jetCand->phi[i]-trkCand->phi[j];

						while(dphic>(1.5*TMath::Pi())){dphic+= -2*TMath::Pi();}
						while(dphic<(-0.5*TMath::Pi())){dphic+= 2*TMath::Pi();}

						float weightc = evtWeight*(jetCand->weight[i])*(trkCand->weight[j]);
						weight.push_back(weightc);
						dphi.push_back(dphic);
						float detac = jetCand->eta[i]-trkCand->eta[j];
						deta.push_back(detac);
						if(fillMix){
							hc.mixing->Fill(detac, dphic, weightc);	
						}else {
							hc.sig->Fill(detac, dphic, weightc);	
							hc.sig_pTweighted->Fill(detac, dphic, trkCand->pt[j]*weightc);
						}
					}
				}
			}
		template< size_t n>
			void fillJetCand(histCase & hc,  int flag[n]){
				for(int i=0; i<jetCand.flag[i]; i++){
					if(n!=0) if(std::find(flag, flag+n, jetCand->flag[i]) == flag+n) 
						continue;
					hc.jet_pt ->Fill(jetCand->pt.at(i) ,evtWeight*jetCand->weight.at(i));
					hc.jet_eta->Fill(jetCand->eta.at(i),evtWeight*jetCand->weight.at(i));
					hc.jet_phi->Fill(jetCand->phi.at(i),evtWeight*jetCand->weight.at(i));
				}
			}

		candidates *jetCand, *trkCand;
		std::vector<std::pair<float, float> etap, phip, weightp;
		std::vector<float> deta, dphi, weight;
		float evtWeight = 1;
};

#endif
