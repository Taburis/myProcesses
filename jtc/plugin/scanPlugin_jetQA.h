#ifndef scanPlugin_jetQA_H
#define scanPlugin_jetQA_H

#include "myProcesses/jtc/plugin/simpleScanner.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/toolkit.h"
#include "myProcesses/jtc/plugin/xTagger.h"
#include "myProcesses/jtc/plugin/QATools.h"


template <typename cfg, typename event>
class scanPlugin_jetQA : public scanPlugin<cfg,event>{
	public :
		scanPlugin_jetQA():scanPlugin<cfg,event>(){}
		~scanPlugin_jetQA(){}

		void beginJob(){this->ss->doJet= 1;};
		void run(){
			int jcent = this->ss->getCentIndex();
			if(jcent<0) return;
			float weight = this->getEvtWeight();
			for(int i=0; i<this->evt->nJet(); ++i){
				if(this->_cfg->cuts.recoJetCut(this->evt, i))continue;
				xTagger tag = jetSelection(this->evt, i);
				for(auto & it : jetSets){
					it->fillHist(tag,jcent,this->evt->jetpt[i],this->evt->jeteta[i],this->evt->jetphi[i],weight);
				}
			}
		}

		void endJob(){
			wf = TFile::Open(output,"recreate");	
			this->hm->write(wf);
		}

		void addJetSet(TString name, int bit){
			auto js = new jetQASet(name, bit, this->_cfg->pset.ncent, this->hm);
			jetSets.emplace_back(js);
		}

		xTagger (*jetSelection)(event* em, int j)=0;

		TString _name = "bTagJetQA", output="jetQAResult.root";
		std::vector<jetQASet*> jetSets;
		TFile *wf;
};

#endif
