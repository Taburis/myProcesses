
#ifndef PRODUCER_JETQA_H
#define PRODUCER_JETQA_H

#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"

template <typename event, typename config>
class producerJetQA : public producerBase<event,config>{
        public :
                producerJetQA(const char* name):producerBase<event, config>(name){}
                ~producerJetQA(){}

                bool linkFrame(liteFrame<event, config> *frame){frame->doJet= 1; return 0;}
		bool initialCheck(){return 0;}
                void beginJob(){}
                void run(){
                        int jcent = this->_frame->getCentIndex();
                        if(jcent<0) return;
                        float weight = this->getEvtWeight();
                        for(int i=0; i<this->evt->nJet(); ++i){
                                xTagger tag = this->_cfg->src->tagRecoJet(this->evt, i);
				if(tag.tag == 0) continue;
                                for(auto & it : jetSets){
                                        it->fillHist(tag,jcent,this->evt->jetpt[i],this->evt->jeteta[i],this->evt->jetphi[i],weight);
                                }
                        }
                }

                void endJob(){
                        //wf = TFile::Open(output,"recreate");
                        //this->hm->write(wf);
                }

                void addJetSet(TString name, int bit){
                        auto js = new jetQASet(name, bit, this->_cfg->ps->ncent, this->hm);
                        jetSets.emplace_back(js);
                }

                xTagger (*jetSelection)(event* em, int j)=0;

                std::vector<jetQASet*> jetSets;
                TFile *wf;
};


#endif
