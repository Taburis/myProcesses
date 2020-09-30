
#ifndef PRODUCER_JETQA_H
#define PRODUCER_JETQA_H

#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"

template <typename event, typename config>
class producerEvtQA : public producerBase<event,config>{
	public :
		producerEvtQA(const char* name):producerBase<event, config>(name){}
		~producerEvtQA(){}

		bool linkFrame(liteFrame<event, config> *frame){frame->doJet= 1; return 0;}
		bool initialCheck(){return 0;}
		void beginJob(){
			hvz = this->hm->template regHist<TH1D>("evtInfo/vzInfo", "vz distribution", 200,-20,20);	
			hcent = this->hm->template regHist<TH1D>("evtInfo/centInfo","hiBin distribution" ,200,0,200);	
			if(this->_cfg->ps->isMC){
				hpthat = this->hm->template regHist<TH1D>("evtInfo/pthatInfo","pT hat distribution",100, 0, 400);	
				hweight = this->hm->template regHist<TH1D>("evtInfo/weightDist", "weight statistics",50, 0, 50);	
			}
		}
		void run(){
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

		void endJob(){}


		xTagger (*jetSelection)(event* em, int j)=0;

		std::vector<jetQASet*> jetSets;
		TFile *wf;
		TH1D* hvz, *hcent, *hpthat, *hweight;
};


#endif
