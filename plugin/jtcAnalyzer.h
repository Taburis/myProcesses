
#include "histManager.h"
#include "xAxis.h"
#include "eventMap_hiForest.h"
#include "jtcUti.h"
#include <cstdarg>
#include "TMath.h"
#include "TFile.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

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
	struct dataWrapper {
		std::vector<candidate> * jetCand, *trks=nullptr;
		jetType jetid; trkType trkid;
		histCase *hc;
		float evtW; 
	};
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
	virtual std::vector<candidate>* recoTrkSelections(eventMap *em){
		std::vector<candidate>* cands = new std::vector<candidate>();
		for(int i=0; i< em->nTrk(); ++i){
			if(recoTrkCuts(em, i)) continue;
			candidate cc(trkType::inclTrk, em->trkpt[i], em->trketa[i], em->trkphi[i], 1);	
		}
		return cands;
	}
	virtual std::vector<candidate>* genParticleSelections( eventMap *em){
		std::vector<candidate>* cands = new std::vector<candidate>();
		for(int i=0; i< em->nGP(); ++i){
			if(genParticleCuts(em, i)) continue;
			candidate cc(trkType::inclTrk, em->gppt(i),em->gpeta(i),em->gpphi(i),1);
			cands->push_back(cc);
		}
		return cands;
	}
	//		virtual std::vector<candidate>* recoJtSelections(candidates *jc, eventMap *em){
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
	// the flow to organize the correlation is:
	// get jet and trk collection ----> dataWrapper ------> producer -----> done by filling histogram 
	// mixing is the same:
	// select jet sets -----> dataWrapper ------> mixing producer -----> done by filling histogram 
	//
	virtual void produce(histCase &hc, std::vector<candidate>*jetCand, jetType flag1, std::vector<candidate> *trkCand, trkType flag2, float evtWeight, bool fillMix = 0){
		for(auto i = 0;i<jetCand->size(); i++){
			if(jetCand->at(i).flag != flag1) continue;
			if(!fillMix){
				hc.jet_pt[centj]->Fill(jetCand->at(i).pt, evtWeight*(jetCand->at(i).weight));
				hc.jet_eta[centj]->Fill(jetCand->at(i).eta, evtWeight*(jetCand->at(i).weight));
				hc.jet_phi[centj]->Fill(jetCand->at(i).phi, evtWeight*(jetCand->at(i).weight));
			}
			for(auto j = 0;j<trkCand->size(); j++){
				if(trkCand->at(j).flag != flag2) continue;

				int ptj = ptax->findBin(safeValue(trkCand->at(j).pt,ptbins[nPt]));
				float dphic=(jetCand->at(i)).phi-(trkCand->at(j)).phi;

				while(dphic>(1.5*TMath::Pi())){dphic+= -2*TMath::Pi();}
				while(dphic<(-0.5*TMath::Pi())){dphic+= 2*TMath::Pi();}

				float weightc = evtWeight*(jetCand->at(i).weight)*(trkCand->at(j).weight);
				float detac = jetCand->at(i).eta-trkCand->at(j).eta;
				//cout<<"filling: "<<ptj<<" : "<<detac<<" : "<<dphic<<endl;
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
		if(in > max) return max-1;
		return in;
	}
	void fillEventInfo(float evtWeight = 1){
		hvz->Fill(em->vz, evtWeight);
		if(isMC) hpthat->Fill(em->pthat, evtWeight);
		if(!ispp)hcent->Fill(em->hiBin, evtWeight);
	}
	void init(){
		hm = new histManager();
		ptax = new xAxis(nPt, ptbins);
		centax= new xAxis(nCent, centbins);
		hvz = hm->regHist<TH1D>("vzInfo", "", 200, -20, 20);
		if(!ispp)hcent = hm->regHist<TH1D>("centInfo","",  50, 0, 200);
		if(isMC) hpthat = hm->regHist<TH1D>("pthatInfo", "", 100, 0, 400);
		quickHistReg("inclJet_", "GenJet_GenTrk", hm, inclCase, nPt, nCent);
		quickHistReg("trueBJet_", "GenJet_GenTrk", hm, trueBCase, nPt, nCent);
		ncent_mix = ispp ? 1 : 40;
		if(domixing) scanMixingTable();
	}
	void write(std::string name){
		auto wf = TFile::Open(name.c_str(), "recreate");
		wf->cd();
		hm->write();
		wf->Close();
	}
	bool initialCheck(){
		// the code won't run if the initial check failed
		if(isMC){
			if( jetPtWeight == 0) {
				std::cout<<"jet pT re-weight function is missing"<<std::endl;
				return 1;
			}
			if( evtWeight == 0) {
				std::cout<<"event weight function is missing"<<std::endl;
				return 1;
			}
		}
		return 0;
	}

	bool evtCut(eventMap *em){
		if(em->checkEventFilter()) return 1;
		if(TMath::Abs(em->vz) > 15) return 1;
		return 0;
	}

	void scanMixingTable(){
		mixTable = new std::vector<unsigned int>*[nvz_mix*ncent_mix];
		for(int i=0; i<nvz_mix;++i){
			for(int j=0; j<ncent_mix; j++){
				mixTable[i+nvz_mix*j]=new std::vector<unsigned int>();
			}
		}
		Long64_t nentries = mixem->evtTree->GetEntries();
		std::cout<<"total events in mixing = "<<nentries<<std::endl;
		for(Long64_t jevt = 0; jevt<nentries; ++jevt){
			if(jevt%10000 == 0 ) std::cout<<"scan "<<jevt<<" events in mixing tree..."<<std::endl;
			mixem->getEvent(jevt);
			//need to implement the PbPb case event cut
			if(evtCut(mixem)) continue;
			//cout<<mixem->vz<<endl;
			int ivz = floor(mixem->vz+15);
			int ihibin = ispp ? 0: floor(float(mixem->hiBin)/5);
			//cout<<"vz = "<<mt->vz<<", ivz = "<<ivz<<endl;
			if(mixTable[ivz+nvz_mix*ihibin]->size()< 100)mixTable[ivz+nvz_mix*ihibin]->push_back(jevt);
		}
		std::cout<<"vz statitcs problem: "<<endl;
		for(int i= 0; i<nvz_mix; ++i){
			for(int j= 0; j<ncent_mix; ++j){
				if(mixTable[i+nvz_mix*j]->size()<2) std::cout<<-15+i<<" < vz < "<<-14+i<<"; "
					<<j*5<<" < cent < "<<j*5+5<<": "<<mixTable[i+nvz_mix*j]->size()<<std::endl;
			}
		}
		std::cout<<"vz report done."<<std::endl;
	}

	void makeWrapper(histCase &hc, std::vector<candidate>*jts, jetType jetid,float weight){
		dataWrapper dw;
		dw.jetCand = jts; dw.hc=&hc; dw.jetid=jetid;dw.evtW=weight;
		mixingCollection.push_back(dw);
		return;
	}

	void mixingLoop(){
		int kevt = 0;
		int vzIndex = floor(em->vz+15);
		int centIndex = ispp ? 0 : floor(float(em->hiBin)/5);
		if(mixTable[vzIndex+centIndex*nvz_mix]->size()<2){
			if(vzIndex == 29) vzIndex = 28;// shift mixing vz from 14, 15 to 13, 14; 
			else if(vzIndex == 0) vzIndex = 1;// shift mixing vz from -14,-15 to -13, -14; 
			else if(centIndex>0) centIndex=centIndex-1;  // shift the most prepheral to next prepheral
		}
		if(mixTable[vzIndex+centIndex*nvz_mix]->size()==0) return;
		for(int kmix = 0; kmix<nPerTrig; ++kmix){
			//cout<<kmix<<endl;
			if(kevt == mixTable[vzIndex+centIndex*nvz_mix]->size()) kevt = 0;
			//cout<<"vz = "<<vz<<", index = "<<vzIndex+centIndex*ncent_mix<<endl;
			//cout<<"index = "<<mixTable[vzIndex+centIndex*ncent_mix]->at(kevt)<<endl;
			Long64_t index = mixTable[vzIndex+centIndex*nvz_mix]->at(kevt);
			if(index == voidIndex) continue; // to avoid the auto correlation in any case
			mixem->evtTree->GetEntry(index);
			//cout<<"current vz: "<<vz<<", mix vz: "<<mt->vz<<endl;
			auto gpmix = genParticleSelections(mixem);
			for( auto & it:mixingCollection){
				//std::cout<<"!"<<std::endl;
				produce(*(it.hc),it.jetCand,it.jetid,gpmix,trkType::inclTrk,it.evtW, 1);
			}
			gpmix->clear();
			kevt++;
		}
	}

	float (*evtWeight)(eventMap *em) = 0;
	float (*jetPtWeight)(float pt) = 0; //return 1 to skip
	void loop();
	bool isMC, ispp;
	bool domixing = 0;
	eventMap * em, *mixem;
	histManager *hm;
	int nPt=6, nCent= 1;
	Long64_t nevt = -1; 
	xAxis *ptax , *centax; 
	TH1D* hvz, *hcent, *hpthat;
	std::vector<unsigned int>** mixTable;
	std::vector<dataWrapper> mixingCollection;


	int nPerTrig = 20;
	int ncent_mix, nvz_mix = 30;
	float *ptbins     =0;
	float *centbins   =0;
	TString *ptLabel  =0;
	TString *centLabel=0;
	histCase inclCase, trueBCase;

	//global variables for transfer data
	int centj =0;
	Long64_t voidIndex;
};

void jtcAnalyzer::loop(){
	if(initialCheck()) return;
	init();
	Long64_t nentries = nevt < 0 ? em->evtTree->GetEntriesFast() : nevt;
	for(Long64_t jentry = 0; jentry< nentries; ++jentry){
		if(jentry%1000 ==0 ) std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
		em->getEvent(jentry);	
		if(evtCut(em)) continue;
		float evtW= 1;
		if(isMC) evtW= evtWeight(em);
		fillEventInfo(evtW);
		auto gj = genJtSelections(em);
		auto gp = genParticleSelections(em);
		produce(trueBCase, gj, jetType::trueBJet, gp, trkType::inclTrk, evtW);
		produce(inclCase,  gj, jetType::inclJet, gp, trkType::inclTrk, evtW);
		if(domixing){
			voidIndex = jentry;
			// makeWrapper arg: histCase , vector<candidate> jts, jetType, evtWeight
			makeWrapper(trueBCase, gj, jetType::trueBJet, evtW);
			makeWrapper(inclCase, gj, jetType::inclJet, evtW);
			mixingLoop();
		}
		//don't forget to clear the space
		gp->clear(); 
		gj->clear();
	}
	write("correlation.root");
}
