
#ifndef PRODUCER_JETQA_H
#define PRODUCER_JETQA_H

#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"

namespace jetQA{
        Int_t nptbin = 20;
        const Double_t ptbin[21] = {110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360, 380, 400, 432, 500};
        const Double_t detabin[24] ={-3.5, -3, -2.5,-2.,-1.5, -1., -0.8, -0.6, -0.4, -0.3, -0.2, -0.1, 0.1, 0.2, 0.3, 0.4, 0.6, 0.8, 1., 1.5,2.,2.5, 3, 3.5};
        const Double_t dphibin[18] ={-1.50796, -1.00531,-0.879646, -.75398, -0.628319,-0.502655, -0.376991, -0.251327, -0.125664, 0.125664, 0.251327, 0.376991, 0.502655, 0.628319,.75398, 0.879646, 1.00531,1.50796};
        int nJetIDptbin = 17;
        Double_t jetIDptbin[18] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 250, 300};
}

class jetQASet : public xTSetBase {
	public :
		jetQASet():xTSetBase(){}
		jetQASet(TString name0, histManager *hm, int tag0, int ncent, const float *centbin): xTSetBase(name0, hm, tag0, ncent, centbin){
		}
		void init(){
			jetpt  = regHist<TH1D>("jetpt" , "", jetQA::nptbin, jetQA::ptbin);
			jeteta = regHist<TH1D>("jeteta", "", 50, -2 ,2);
			jetphi = regHist<TH1D>("jetphi", "", 36, -TMath::Pi(), TMath::Pi());
			jetEnergyPt = regHist<TH2D>("JECPt", "", jetQA::nptbin, (Double_t*)jetQA::ptbin, 100, 0, 3);
			jetEnergyEta = regHist<TH2D>("JECEta", "", 50, -2, 2, 100, 0, 3);
			if(doJetID){
				jetID_trkMax    = regHist<TH1D>("trkPtMax"  ,"trkPtMax"  ,30, 0, 50);
				jetID_trkSum    = regHist<TH1D>("trkPtSum"  ,"trkPtSum"  ,jetQA::nJetIDptbin, jetQA::jetIDptbin);
				jetID_photonSum = regHist<TH1D>("photonSum" ,"photonSum" ,jetQA::nJetIDptbin, jetQA::jetIDptbin);
				jetID_neutralSum= regHist<TH1D>("neutralSum","neutralSum",jetQA::nJetIDptbin, jetQA::jetIDptbin);
				jetID_chargedSum= regHist<TH1D>("chargedSum","chargedSum",jetQA::nJetIDptbin, jetQA::jetIDptbin);                
				jetID_eSum      = regHist<TH1D>("eSum"      ,"eSum"      ,jetQA::nJetIDptbin, jetQA::jetIDptbin);                
				jetID_hcalSum   = regHist<TH1D>("hcalSum"   ,"hcalSum"   ,50, 0, 50);
				jetID_ecalSum   = regHist<TH1D>("ecalSum"   ,"ecalSum"   ,50, 0, 50);
			}
		}
		~jetQASet(){};

		void produceJEC(){
			jecptMu  = new TProfile*[ncent];
			jecetaMu = new TProfile*[ncent];
			for(int i=0; i<ncent; ++i){
				jecptMu[i] = jetEnergyPt[i]->ProfileX(_name+"_"+jetEnergyPt[i]->GetName()+"_pfx");
				jecetaMu[i]= jetEnergyEta[i]->ProfileX(_name+"_"+jetEnergyEta[i]->GetName()+"_pfx");
			}
		}

		void loadSet(TFile *f){
			if(!isInit){
				std::cout<<"Jet Set haven't been initialized yet!"<<std::endl;
				return;
			}
			jetpt  = loadHist<TH1D>("jetpt" , f);
			jeteta = loadHist<TH1D>("jeteta", f);
			jetphi = loadHist<TH1D>("jetphi", f);
			jetEnergyPt = loadHist<TH2D>("JECPt", f);
			jetEnergyEta= loadHist<TH2D>("JECEta", f);
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
				//cout<<"hiBin:"<<evt->hiBin<<endl;
				//cout<<"jcent:"<<jcent<<endl;
				if(jcent<0) return;
				jetpt [jcent]->Fill(evt->jetpt[i], weight);
				jeteta[jcent]->Fill(evt->jeteta[i], weight);
				jetphi[jcent]->Fill(evt->jetphi[i], weight);
				float rpt = evt->jetpt[i]/evt->genjetpt[i];
				//cout<<rpt<<endl;
				jetEnergyPt[jcent]->Fill(evt->genjetpt[i],rpt, weight);
				jetEnergyEta[jcent]->Fill(evt->genjeteta[i],rpt, weight);
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
		TH2D **jetEnergyPt, **jetEnergyEta; //for studying JEC and JER
		TProfile **jecptMu , **jecetaMu;
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
