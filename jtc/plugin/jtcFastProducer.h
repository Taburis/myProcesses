
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "TMath.h"
#include "TFile.h"
#include "TBenchmark.h"
#include "myProcesses/jtc/plugin/histManager.h"
#include "myProcesses/jtc/plugin/xAxis.h"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/plugin/xTagger.h"

enum jetType {inclJet, trueBJet};
enum trkType {inclTrk};

class candidate {
	public : 
		candidate(){};
		candidate(xTagger tg, float pt0,float eta0,float phi0, float w= 1){ 
			set(tg, pt0,eta0,phi0,w);
		}
		void copy(candidate &c){
			pt=c.pt;eta=c.eta;phi=c.phi;weight=c.weight;tag=c.tag;isValid = c.isValid;
		}
		candidate & operator = (candidate &rhs){
			this->copy(rhs);
			return (*this);
		}
		~candidate(){};
		void set(xTagger &tg, float pt0,float eta0,float phi0, float w= 1){
			pt=pt0; 
			eta=eta0;
			phi=phi0;
			tag= tg;
			weight=w;
			isValid = 1;
		}
		void clear(){isValid = 0;}

		float pt, eta, phi, weight;
		xTagger tag;
		bool isValid= 0;
};

class jtcFastProducer{
	struct jtcTag {
		xTagger jetTag, trkTag; histCase *hc;
	};
	public : 
	jtcFastProducer(eventMap *em0){ 
		em = em0; isMC=em->isMC;
	}
	~jtcFastProducer(){}
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
//		if(em->ref_jetpt[j] < 50.0) return 1;
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
	//virtual std::vector<candidate>* recoTrkSelections(eventMap *em){
	//	std::vector<candidate>* cands = new std::vector<candidate>();
	//	for(int i=0; i< em->nTrk(); ++i){
	//		if(recoTrkCuts(em, i)) continue;
	//		candidate cc(trkType::inclTrk, em->trkpt[i], em->trketa[i], em->trkphi[i], 1);	
	//	}
	//	return cands;
	//}
	virtual void genParticleSelections(std::vector<candidate>&cands, eventMap *em){
		cands.reserve(em->nGP());
		for(int i=0; i< em->nGP(); ++i){
			if(genParticleCuts(em, i)) continue;
			xTagger tag;
			tag.addTag(trkType::inclTrk);
			candidate cc(tag, em->gppt(i),em->gpeta(i),em->gpphi(i),1);
			cands.emplace_back(cc);
		}
	}
	//		virtual std::vector<candidate>* recoJtSelections(candidates *jc, eventMap *em){
	//			for(int i=0; i< em->nJet(); ++i){
	//				if(recoJtCuts(em, i)) continue;
	//				float weight = isMC ? jetPtWeight(em->jetpt[i]) : 1;
	//				jc->add(em->jetpt[i], em->jeteta[i], em->jetphi[i], 1, weight );
	//			}
	//		}

	virtual void genJtSelections(std::vector<candidate>&cands, eventMap *em){
		// if the jets have n types, need to add n times...
		cands.reserve(em->nGenJet());
		for(int i=0; i< em->nGenJet(); ++i){
			if(genJtCuts(em, i)) continue;
			//add incl jet tag
			float weight = isMC ? jetPtWeight(em->genjetpt[i]) : 1;
			xTagger tag; tag.addTag(jetType::inclJet);

			//add true b jet tag
			int index = em->genMatchIndex[i];
			if(index > -1) if(TMath::Abs(em->flavor_forb[index]) == 5) 
				tag.addTag(jetType::trueBJet);
			candidate cc2(tag, em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
			cands.emplace_back(cc2);
			//-------------
		}
	}
	void quickHistReg(TString cap, TString dsname,  histManager *h, histCase &hc, int nPt, int nCent){
		int nHistoBinsX = 500;
		int nHistoBinsY = 200;
		hc.sig= new TH2D*[nPt*nCent];
		hc.sig_pTweighted= new TH2D*[nPt*nCent];
		hc.mixing= new TH2D*[nPt*nCent];
		TString tmp, name;
		name = cap+"_"+dsname;
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
		for(int j=0; j<nCent; ++j){
			tmp = centLabel[j]+" to "+centLabel[j+1];
			hc.jet_pt[j] = hm->regHist<TH1D>(name+Form("_corrpt_%d",j), tmp, nbin, newbin);
			hc.jet_eta[j] = hm->regHist<TH1D>(name+Form("_eta_%d",j), tmp, 100, -2.0, 2.0);
			hc.jet_phi[j] = hm->regHist<TH1D>(name+Form("_phi_%d",j), tmp, 72, -TMath::Pi(), TMath::Pi());
		}
	}

	virtual void fillJetKinematic(std::vector<candidate>&jetCand, float evtWeight){
		for(auto i = 0;i<jetCand.size(); i++){
			for(int k=0; k<jtcList.size(); ++k){
				if(!(jetCand[i].tag.select(jtcList[k].jetTag))) continue;
				jtcList[k].hc->jet_pt[centj]->Fill(jetCand [i].pt , (jetCand[i].weight)*evtWeight);
				jtcList[k].hc->jet_eta[centj]->Fill(jetCand[i].eta, (jetCand[i].weight)*evtWeight);
				jtcList[k].hc->jet_phi[centj]->Fill(jetCand[i].phi, (jetCand[i].weight)*evtWeight);
			}
		}
	}


	virtual void fillHistCase(histCase &hc, candidate&jet, candidate&trk, float evtWeight, bool fillMix){
		int ptj = ptax->findBin(safeValue(trk.pt,ptbins[nPt]));
		float dphic=(jet).phi-(trk).phi;
		while(dphic>(1.5*TMath::Pi())){dphic+= -2*TMath::Pi();}
		while(dphic<(-0.5*TMath::Pi())){dphic+= 2*TMath::Pi();}

		float weightc = evtWeight*(jet.weight)*(trk.weight);
		float detac = jet.eta-trk.eta;
		//cout<<"filling: "<<ptj<<" : "<<detac<<" : "<<dphic<<endl;
		if(fillMix){
			hc.mixing[ptj+nPt*centj]->Fill(detac, dphic, weightc);
		}else {
			hc.sig[ptj+nPt*centj]->Fill(detac, dphic, weightc);
			hc.sig_pTweighted[ptj+nPt*centj]->Fill(detac, dphic, trk.pt*weightc);
		}
	}

	bool checkJtcPair(jtcTag &secl, candidate&jet,candidate&trk){
		return (jet.tag.select(secl.jetTag) && trk.tag.select(secl.trkTag));
	}

	virtual void produce(std::vector<candidate>&jetCand, std::vector<candidate>&trkCand,float evtWeight, bool fillMix = 0){
		if(!fillMix) fillJetKinematic(jetCand, evtWeight);
		for(int j = 0;j<trkCand.size(); j++){
			for(int i = 0;i<jetCand.size(); i++){
				for(int k=0; k<jtcList.size(); ++k){
					if(checkJtcPair(jtcList[k], jetCand[i], trkCand[j]))
						fillHistCase(*(jtcList[k].hc),jetCand[i], trkCand[j], evtWeight,fillMix);
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
		quickHistReg("inclJet", "GenJet_GenTrk", hm, inclCase, nPt, nCent);
		quickHistReg("trueBJet", "GenJet_GenTrk", hm, trueBCase, nPt, nCent);
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
		//if(em->pthat < 50) return 1;
		return 0;
	}

	void scanMixingTable(){
		mixTable = new std::vector<unsigned int>*[nvz_mix*ncent_mix];
		for(int i=0; i<nvz_mix;++i){
			for(int j=0; j<ncent_mix; j++){
				mixTable[i+nvz_mix*j]=new std::vector<unsigned int>();
				mixTable[i+nvz_mix*j]->reserve(5);
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
			if(mixTable[ivz+nvz_mix*ihibin]->size()< 50)mixTable[ivz+nvz_mix*ihibin]->emplace_back(jevt);
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

	void linkMixingTarget(std::vector<candidate>&jetCand){
		mixingCollection.emplace_back(&jetCand);
	}

	void mixingLoop(float evtW){
		int kevt = 0;
		int vzIndex = floor(em->vz+15);
		int centIndex = ispp ? 0 : floor(float(em->hiBin)/5);
		if(mixTable[vzIndex+centIndex*nvz_mix]->size()<2){
			if(vzIndex == 29) vzIndex = 28;// shift mixing vz from 14, 15 to 13, 14; 
			else if(vzIndex == 0) vzIndex = 1;// shift mixing vz from -14,-15 to -13, -14; 
			else if(centIndex>0) centIndex=centIndex-1;  // shift the most prepheral to next prepheral
		}
		if(mixTable[vzIndex+centIndex*nvz_mix]->size()==0) return;

		std::vector<candidate> gpmix;
		xTagger mixTrkTag(trkType::inclTrk);
		for(int kmix = 0; kmix<nPerTrig; ++kmix){
			if(kevt == mixTable[vzIndex+centIndex*nvz_mix]->size()) kevt = 0;
			//cout<<"vz = "<<vz<<", index = "<<vzIndex+centIndex*ncent_mix<<endl;
			//cout<<"index = "<<mixTable[vzIndex+centIndex*ncent_mix]->at(kevt)<<endl;
			Long64_t index = mixTable[vzIndex+centIndex*nvz_mix]->at(kevt);
			if(index == voidIndex) continue; // to avoid the auto correlation in any case
			mixem->evtTree->GetEntry(index);
			//cout<<"current vz: "<<vz<<", mix vz: "<<mt->vz<<endl;
			genParticleSelections(gpmix, mixem);
			for( auto & it:mixingCollection){
//				std::cout<<"jet set length: "<<(*(it)).size()<<std::endl;
				produce(*(it), gpmix, evtW, 1);
			}
			//cout<<"mixed "<<kmix<<": "<<gpmix.size()<<endl;
			gpmix.clear();
			kevt++;
		}
	}
	void regJtcPair(xTagger jetTg, xTagger trkTg, histCase &hc){
		jtcTag tg;
		tg.jetTag=jetTg; tg.trkTag=trkTg; tg.hc=&hc;
		jtcList.emplace_back(tg);
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
	std::vector<std::vector<candidate>*> mixingCollection;
	std::vector<jtcTag> jtcList;


	int nPerTrig = 40;
	int ncent_mix, nvz_mix = 30;
	float *ptbins     =0;
	float *centbins   =0;
	TString *ptLabel  =0;
	TString *centLabel=0;
	histCase inclCase, trueBCase;

	//global variables for transfer data
	int centj =0;
	Long64_t voidIndex;
	std::string outputName = "correlation.root";
};

void jtcFastProducer::loop(){
	TBenchmark clock; clock.Start("loop");

	if(initialCheck()) return;
	init();
	Long64_t nentries = nevt < 0 ? em->evtTree->GetEntriesFast() : nevt;

	xTagger inclJetTag(jetType::inclJet), trueBJetTag(jetType::trueBJet), inclTrkTag(trkType::inclTrk);
	std::vector<candidate> gj, gp;
	regJtcPair(inclJetTag, inclTrkTag, inclCase);
	regJtcPair(trueBJetTag, inclTrkTag, trueBCase);
	linkMixingTarget(gj);
	for(Long64_t jentry = 0; jentry< nentries; ++jentry){
		if(jentry%1000 ==0 ){
			std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
			clock.Show("loop");
			clock.Reset();
			clock.Start("loop");
		}
		em->getEvent(jentry);	
		if(evtCut(em)) continue;
		float evtW= 1;
		if(isMC) evtW= evtWeight(em);
		fillEventInfo(evtW);
		genJtSelections(gj, em);
//		std::cout<<"length: "<<gj.size()<<endl;
		genParticleSelections(gp, em);
		produce(gj, gp, evtW);
		//free the track memory before the mixing loop;
		gp.clear();
		if(domixing){
			mixingLoop(evtW);
		}
		//don't forget to clear the space
		gj.clear();
	}
	write(outputName);
}


