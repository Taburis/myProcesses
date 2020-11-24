
#ifndef PRODUCER_JETID_H
#define PRODUCER_JETID_H

#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"

template <typename event, typename config>
class producerJetID : public producerBase<event,config>{
	public :
		producerJetID(const char* name):producerBase<event, config>(name){}
		~producerJetID(){}

		bool linkFrame(liteFrame<event, config> *frame){frame->doJet= 1; this->evt->addJetID =1; return 0;}
		bool initialCheck(){return 0;}
		void beginJob(){
			TString folder = "jetIDQA";
			trkMaxSet = new qaTSet<TH1D>(folder+"/trkPtMax","trkPtMax",this->hm, 1, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 30, 0, 30);
			trkSumSet = new qaTSet<TH1D>(folder+"/trkPtSum","trkPtSum",this->hm, 1, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 200);
			hcalSumSet = new qaTSet<TH1D>(folder+"/hcalSum","hcalSum",this->hm, 1, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 400);
			ecalSumSet = new qaTSet<TH1D>(folder+"/ecalSum","ecalSum",this->hm, 1, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 400);
			photonSumSet = new qaTSet<TH1D>(folder+"/photonSum","photonSum",this->hm, 1, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 200);
			neutralSumSet = new qaTSet<TH1D>(folder+"/neutralSum","neutralSum",this->hm, 1, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 200);
			chargedSumSet = new qaTSet<TH1D>(folder+"/chargedSum","chargedSum",this->hm, 1, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 200);
			eSumSet = new qaTSet<TH1D>(folder+"/eSum","eSum",this->hm, 1, this->_cfg->ps->ncent, this->_cfg->ps->centbin, 50, 0, 200);
		}
		void run(){
			int jcent = this->_frame->getCentIndex();
			if(jcent<0) return;
			float weight = this->getEvtWeight();
			for(int i=0; i<this->evt->nJet(); ++i){
				xTagger tag = this->_cfg->src->tagRecoJet(this->evt, i);
				if(tag.tag == 0) continue;
				int hibin = this->evt->hiBin;
				trkMaxSet ->fill(tag, hibin, this->evt->trkPtMax[i], weight);
				trkSumSet ->fill(tag, hibin, this->evt->trkPtSum[i], weight);
				ecalSumSet->fill(tag, hibin, this->evt->ecalSum[i], weight);
				hcalSumSet->fill(tag, hibin, this->evt->hcalSum[i], weight);
				neutralSumSet->fill(tag, hibin, this->evt->neutralPfPtSum[i], weight);
				chargedSumSet->fill(tag, hibin, this->evt->chargedPfPtSum[i], weight);
				photonSumSet->fill(tag, hibin, this->evt->photonPtSum[i], weight);
				eSumSet->fill(tag, hibin, this->evt->ePtSum[i], weight);
			}
		}

		void endJob(){
		}

		xTagger (*jetSelection)(event* em, int j)=0;

		qaTSet<TH1D> *trkMaxSet, *trkSumSet, *hcalSumSet, *ecalSumSet, *photonSumSet, *eSumSet, *chargedSumSet, *neutralSumSet;
		TFile *wf;
};


#endif
