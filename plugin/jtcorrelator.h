
#ifndef CORRELATOR_H
#define CORRELATOR_H

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
		void clear(){
			jetCand->clear(); trkCand->clear();
			deta.clear(); dphi.clear(), weight.clear();
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

					weight.push_back(jetCand->weight[i]*trkCand->weight[j]);
					dphi.push_back(dphic);
					deta.push_back(jetCand->eta[i]-trkCand->eta[j]);
				}
			}
		}
		void fillTH2(TH2* h, float evtWeight=1){
			for(auto i=0; i< deta.size(); ++i){
				h->Fill(deta[i], dphi[i], weight[i]*evtWeight);
			}
		}

		candidates *jetCand, *trkCand;
		std::vector<float> deta, dphi, weight;
};

#endif
