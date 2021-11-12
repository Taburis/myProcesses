
#ifndef PRODUCER_EVTQA_H
#define PRODUCER_EVTQA_H

#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"


class evtTree{
	public : 
		evtTree(){};
		~evtTree(){};
		void makeTree(){
			t = new TTree("mixing_tree", "");
			t->Branch("vz", &vz);
			t->Branch("hiBin", &hiBin);
			if(isMC){
				t->Branch("pthat", &pthat);			
				t->Branch("weight", &weight);			
			}
		};
		void load(TFile *f, bool ismc){
			t = (TTree*)f->Get("evtTree");
			t->SetBranchAddress("vz", &vz);
			t->SetBranchAddress("hiBin", &hiBin);
			if(isMC){
				t->SetBranchAddress("pthat", &pthat);
				t->SetBranchAddress("weight", &weight);
			}
		}
		void fill(float vz0, float hibin, float pthat0=1, float w=1){
			vz = vz0; hiBin = hibin; pthat = pthat0; weight = w;
			t->Fill();
		}
		void write(){
			t->Write();
		}

		float vz, pthat, weight;
		int hiBin;

		TTree *t;
		bool isMC=0;
};

template <typename event, typename config>
class producerEvtQA : public producerBase<event,config>{
	public :
		producerEvtQA(const char* name):producerBase<event, config>(name){}
		~producerEvtQA(){}

		bool linkFrame(liteFrame<event, config> *frame){frame->doTrk= 1;frame->doJet= 1; return 0;}
		bool initialCheck(){return 0;}
		void addTriggerCheckList(int ntri, std::string *list){
			ntrig = ntri; trigList = list;
		}
		void beginJob(){
			if(makeMiniEvtTree){
				t = new evtTree();
				t->isMC = this->_cfg->ps->isMC;
				t->makeTree();
				if(ntrig>0) this->evt->copyTriggerInfo(t->t, ntrig, trigList);
				return;
			}
			hvz = this->hm->template regHist<TH1D>("evtInfo/vzInfo", "vz distribution", 200,-20,20);	
			hcent = this->hm->template regHist<TH1D>("evtInfo/centInfo","hiBin distribution" ,200,0,200);	
			htrkmap[0] = this->hm->template regHist<TH2D>("trkMap0","track map" ,100,-3, 3, 100, -TMath::Pi(), TMath::Pi());	
			htrkmap[1] = this->hm->template regHist<TH2D>("trkMap1","track map" ,100,-3, 3, 100, -TMath::Pi(), TMath::Pi());	
			htrkmap[2] = this->hm->template regHist<TH2D>("trkMap2","track map" ,100,-3, 3, 100, -TMath::Pi(), TMath::Pi());	
			if(this->_cfg->ps->isMC){
				hpthat = this->hm->template regHist<TH1D>("evtInfo/pthatInfo","pT hat distribution",100, 0, 400);	
				hweight = this->hm->template regHist<TH1D>("evtInfo/weightDist", "weight statistics",50, 0, 50);	
			}
		}
		void run(){
			if(makeMiniEvtTree){
				float phat = this->evt->pthat;
				float w = this->evt->weight;
				t->fill(this->evt->vz, this->evt->hiBin, phat, w);
				return;
			}
			int jcent = this->_frame->getCentIndex();
			if(jcent<0) return;
			float weight = this->evtWeight;
			hvz->Fill(this->evt->vz, weight);
			hcent->Fill(this->evt->hiBin, weight);
			if(this->_cfg->ps->isMC){
				hpthat->Fill(this->evt->pthat, weight);
				hweight->Fill(weight);
			}
			for(auto jtrk = 0; jtrk<this->evt->nTrk(); jtrk++){
				if(this->evt->trkpt[jtrk]>3) continue;
				htrkmap[jcent]->Fill(this->evt->trketa[jtrk], this->evt->trkphi[jtrk]);	
			}
		}

		void endJob(){
			if(makeMiniEvtTree){
				t->write();
			}
		}

		xTagger (*jetSelection)(event* em, int j)=0;

		TFile *wf;
		TH1D* hvz, *hcent, *hpthat, *hweight;
		TH2D* htrkmap[3];
		bool makeMiniEvtTree = 0;
		int ntrig= 0;
		std::string *trigList = 0;

		evtTree *t;
};


#endif
