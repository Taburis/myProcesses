
#ifndef PRODUCER_EVTQA_H
#define PRODUCER_EVTQA_H

#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"


class evtTree{
	public : 
		evtTree(){};
		~evtTree(){};
		void makeTree(){
			t = new TTree("evtTree", "");
			t->Branch("vz", &vz);
			t->Branch("hiBin", &hiBin);
			if(isMC){
				t->Branch("pthat", &pthat);			
				t->Branch("weight", &weight);			
			}
		};
		void load(TFile *f){
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

		bool linkFrame(liteFrame<event, config> *frame){frame->doJet= 1; return 0;}
		bool initialCheck(){return 0;}
		void beginJob(){
			if(makeMiniEvtTree){
				t = new evtTree();
				t->isMC = this->_cfg->ps->isMC;
				t->makeTree();
				return;	
			}
			hvz = this->hm->template regHist<TH1D>("evtInfo/vzInfo", "vz distribution", 200,-20,20);	
			hcent = this->hm->template regHist<TH1D>("evtInfo/centInfo","hiBin distribution" ,200,0,200);	
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
			float weight = this->getEvtWeight();
			hvz->Fill(this->evt->vz, weight);
			hcent->Fill(this->evt->hiBin, weight);
			if(this->_cfg->ps->isMC){
				hpthat->Fill(this->evt->pthat, weight);
				hweight->Fill(weight);
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
		bool makeMiniEvtTree = 0;

		evtTree *t;
};


#endif
