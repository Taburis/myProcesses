
#ifndef PRODUCER_JETQA_H
#define PRODUCER_JETQA_H

#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"

class jetQASet : public xTSetBase {
	public :
		jetQASet():xTSetBase(){}
		jetQASet(TString name0, histManager *hm, int tag0, int ncent, const float *centbin): xTSetBase(name0, hm, tag0, ncent, centbin){
		}
		void init(){
			jetpt  = regHist<TH1D>("jetpt" , "", 400, 100, 500);
			jeteta = regHist<TH1D>("jeteta", "", 50, -2 ,2);
			jetphi = regHist<TH1D>("jetphi", "", 36, - TMath::Pi(), TMath::Pi());
			if(doJetID){
				jetID_trkMax    = regHist<TH1D>("trkPtMax"  ,"trkPtMax"  ,30, 0, 50);
				jetID_trkSum    = regHist<TH1D>("trkPtSum"  ,"trkPtSum"  ,50, 0, 300);
				jetID_hcalSum   = regHist<TH1D>("hcalSum"   ,"hcalSum"   ,50, 0, 50);
				jetID_ecalSum   = regHist<TH1D>("ecalSum"   ,"ecalSum"   ,50, 0, 50);
				jetID_photonSum = regHist<TH1D>("photonSum" ,"photonSum" ,50, 0, 300);
				jetID_neutralSum= regHist<TH1D>("neutralSum","neutralSum",50, 0, 300);
				jetID_chargedSum= regHist<TH1D>("chargedSum","chargedSum",50, 0, 300);                
				jetID_eSum      = regHist<TH1D>("eSum"      ,"eSum"      ,50, 0, 100);                
			}
		}
		~jetQASet(){};

		void normalize(){
			
		}

		void loadSet(TFile *f){
			if(!isInit){
				std::cout<<"Jet Set haven't been initialized yet!"<<std::endl;
				return;
			}
			jetpt  = loadHist<TH1D>("jetpt" , f);
			jeteta = loadHist<TH1D>("jeteta", f);
			jetphi = loadHist<TH1D>("jetphi", f);
			if(doJetID){
				jetID_trkMax    = loadHist<TH1D>("trkPtMax"  ,f);
				jetID_trkSum    = loadHist<TH1D>("trkPtSum"  ,f);
				jetID_hcalSum   = loadHist<TH1D>("hcalSum"   ,f);
				jetID_ecalSum   = loadHist<TH1D>("ecalSum"   ,f);
				jetID_photonSum = loadHist<TH1D>("photonSum" ,f);
				jetID_neutralSum= loadHist<TH1D>("neutralSum",f);
				jetID_chargedSum= loadHist<TH1D>("chargedSum",f);
				jetID_eSum      = loadHist<TH1D>("eSum"      ,f);
			}
		}

		template <typename event>
			void fillHist(xTagger &bit, event* evt,int i, float weight){
				// i is the jet index
				if(!(bit.select(tag))) return;
				int jcent = ax->find_bin_in_range(evt->hiBin);
				if(jcent<0) return;
				jetpt [jcent]->Fill(evt->jetpt[i], weight);
				jeteta[jcent]->Fill(evt->jeteta[i], weight);
				jetphi[jcent]->Fill(evt->jetphi[i], weight);
				if(doJetID){
					jetID_trkMax[jcent]->Fill(evt->trkPtMax[i], weight);
					jetID_trkSum[jcent]->Fill(evt->trkPtSum[i], weight);
					jetID_hcalSum[jcent]->Fill(evt->hcalSum[i], weight);
					jetID_ecalSum[jcent]->Fill(evt->ecalSum[i], weight);
					jetID_photonSum[jcent]->Fill(evt->photonPtSum[i], weight);
					jetID_neutralSum[jcent]->Fill(evt->neutralPfPtSum[i], weight);
					jetID_chargedSum[jcent]->Fill(evt->chargedPfPtSum[i], weight);
					jetID_eSum[jcent]->Fill(evt->ePtSum[i], weight);
				}
			}

		TH1D **jetpt, **jeteta, **jetphi;
		bool doJetID=0;
		TH1D **jetID_trkMax, **jetID_trkSum, **jetID_hcalSum, **jetID_ecalSum, **jetID_photonSum, **jetID_neutralSum, **jetID_chargedSum, **jetID_eSum;
};


template <typename event, typename config>
class producerJetQA : public producerBase<event,config>{
	public :
		producerJetQA(const char* name):producerBase<event, config>(name){}
		~producerJetQA(){}

		bool linkFrame(liteFrame<event, config> *frame){frame->doJet= 1; frame->evt->addJetID = doJetID; return 0;}
		void beginJob(){}
		void run(){
			int jcent = this->_frame->getCentIndex();
			if(jcent<0) return;
			float weight = this->getEvtWeight();
			for(int i=0; i<this->evt->nJet(); ++i){
				xTagger tag = this->_cfg->src->tagRecoJet(this->evt, i);
				if(tag.tag == 0) continue;
				for(auto & it : jetSets){
					it->template fillHist<event>(tag, this->evt, i, weight);
				}
			}
		}

		void endJob(){
			//wf = TFile::Open(output,"recreate");
			//this->hm->write(wf);
		}

		void addJetSet(TString name, int bit){
			auto js = new jetQASet(name+"_jetQASet", this->hm, bit, this->_cfg->ps->ncent, this->_cfg->ps->centbin);
			js->doJetID = doJetID;
			js->init();
			jetSets.emplace_back(js);
		}

		xTagger (*jetSelection)(event* em, int j)=0;

		std::vector<jetQASet*> jetSets;
		bool doJetID=0;
};


#endif
