
#ifndef PRODUCER_JETQA_H
#define PRODUCER_JETQA_H

#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"
#include "myProcesses/jtc/plugin/jtcLib.h"


#include "myProcesses/HIN-20-003/residualJEC/JetCorrector.h"

namespace jetQA{
	Int_t nptbin = 23;
	const Double_t ptbin[24] = {80, 90, 100, 110, 120, 136, 152, 168, 184, 200, 216, 232, 248, 264, 280, 296, 312, 328, 344, 360, 380, 400, 432, 500};
	const Double_t detabin[24] ={-3.5, -3, -2.5,-2.,-1.5, -1., -0.8, -0.6, -0.4, -0.3, -0.2, -0.1, 0.1, 0.2, 0.3, 0.4, 0.6, 0.8, 1., 1.5,2.,2.5, 3, 3.5};
	const Double_t dphibin[18] ={-1.50796, -1.00531,-0.879646, -.75398, -0.628319,-0.502655, -0.376991, -0.251327, -0.125664, 0.125664, 0.251327, 0.376991, 0.502655, 0.628319,.75398, 0.879646, 1.00531,1.50796};
	int nJetIDptbin = 17;
	Double_t jetIDptbin[18] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 250, 300};
}

class jetQASet : public xTSetBase {
	public :
		jetQASet():xTSetBase(){}
		jetQASet(TString name0, int tag0, int ncent, const float *centbin): xTSetBase(name0+"_jetQASet", gHM, tag0, ncent, centbin){
			caption = name0;
		}
		void init(){
			jetpt  = regHist<TH1D>(caption+"_jetpt" , "", jetQA::nptbin, jetQA::ptbin);
			jeteta = regHist<TH1D>(caption+"_jeteta", "", 50, -2 ,2);
			jetphi = regHist<TH1D>(caption+"_jetphi", "", 36, -TMath::Pi(), TMath::Pi());
			hpthat  = regHist<TH1D>(caption+"_pthat", "", 100, 10, 1010);
			jetEnergyPt = regHist<TH2D>(caption+"_JECPt", "", jetQA::nptbin, (Double_t*)jetQA::ptbin, 100, 0, 3);
			jetEnergyEta = regHist<TH2D>(caption+"_JECEta", "", 50, -2, 2, 100, 0, 3);
			JEC3D = regHist<TH3D>(caption+"_JEC3D", "", 50, -2.0, 2.0, 36, -TMath::Pi(), TMath::Pi(), 100, 0., 3.0);
		}
		~jetQASet(){};

		void produceJEC(){
			jecptMu  = new TH1D*[ncent];
			jecetaMu = new TH1D*[ncent];
			jecptSigma  = new TH1D*[ncent];
			jecetaSigma = new TH1D*[ncent];
			jec_eta_phi_mu = new TProfile2D*[ncent];
			for(int i=0; i<ncent; ++i){
				jetEnergyPt [i]->FitSlicesY(0, 1, -1);
				jetEnergyEta[i]->FitSlicesY(0, 1, -1);
				jecptMu    [i] = (TH1D*) gDirectory->Get(TString(jetEnergyPt[i]->GetName())+"_1");
				jecetaMu   [i] = (TH1D*) gDirectory->Get(TString(jetEnergyEta[i]->GetName())+"_1");
				jecptSigma [i] = (TH1D*) gDirectory->Get(TString(jetEnergyPt[i]->GetName())+"_2");
				jecetaSigma[i] = (TH1D*) gDirectory->Get(TString(jetEnergyEta[i]->GetName())+"_2");
				jec_eta_phi_mu[i] = JEC3D[i]->Project3DProfile("xy");
			}
		}

		void loadSet(TString name, TFile *f){
			if(!isInit){
				std::cout<<"Jet Set haven't been initialized yet!"<<std::endl;
				return;
			}
			jetpt  = loadHist<TH1D>(name+"_jetpt" , f);
			jeteta = loadHist<TH1D>(name+"_jeteta", f);
			jetphi = loadHist<TH1D>(name+"_jetphi", f);
			hpthat = loadHist<TH1D>(name+"_pthat", f);
			jetEnergyPt = loadHist<TH2D>(name+"_JECPt", f);
			jetEnergyEta= loadHist<TH2D>(name+"_JECEta", f);
			JEC3D= loadHist<TH3D>(name+"_JEC3D", f);
		}

		template <typename event>
			void fillJEC(xTagger &bit, event* evt,int jcent,int genj, int recoj, float weight, JetCorrector &jeccorr){
			//void fillJEC(xTagger &bit, event* evt,int jcent,int genj, int recoj, float weight, jtc::JEUncertTool &tool){
				if(!(bit.select(tag))) return;
				//cout<<"filling"<<endl;
				double eta = evt->jeteta[recoj];
				double average_rho = evt->getUE(eta, 0.4);
				jeccorr.SetJetPT(evt->jetpt[recoj]);
				jeccorr.SetJetEta(eta);
				jeccorr.SetJetPhi(evt->jetphi[recoj]);
				jeccorr.SetRho(average_rho);
				float recopt = jeccorr.GetCorrectedPT();
				//float recopt = evt->jetpt[recoj];
				//float recopt = jeccorr.(evt->jetpt[recoj]);
				//float recopt = tool.smearedPt(evt->jetpt[recoj]);
				float rpt = recopt/evt->genjetpt[genj];
				jetEnergyPt[jcent]->Fill(evt->genjetpt[genj],rpt, weight);
				jetEnergyEta[jcent]->Fill(evt->genjeteta[genj],rpt, weight);
				JEC3D[jcent]->Fill(evt->genjeteta[genj],evt->genjetphi[genj], rpt, weight);
			}
		template <typename event>
			void fillHist(xTagger &bit, event* evt,int jcent, int i, float weight){
				// i is the jet index
				if(!(bit.select(tag))) return;
				//cout<<"hiBin:"<<evt->hiBin<<endl;
				//cout<<"jcent:"<<jcent<<endl;
				jetpt [jcent]->Fill(evt->jetpt[i], weight);
				jeteta[jcent]->Fill(evt->jeteta[i], weight);
				jetphi[jcent]->Fill(evt->jetphi[i], weight);
			}
		template <typename event>
			void fillEvtInfo(event* evt,int jcent, float weight){
				// i is the jet index
				hpthat[jcent]->Fill(evt->pthat, weight);
			}
		void preNormalization(){
			for(int j=0; j<ncent; j++){
				jetpt[j] ->Scale(1.0/hpthat[j]->GetSumOfWeights());
				jeteta[j]->Scale(1.0/hpthat[j]->Integral());
				jetphi[j]->Scale(1.0/hpthat[j]->Integral());
			}
		}

		TH1D **jetpt, **jeteta, **jetphi, **hpthat;
		xTagger evtMask;
		bool doJetID=0;
		TH1D **jetID_trkMax, **jetID_trkSum, **jetID_hcalSum, **jetID_ecalSum, **jetID_photonSum, **jetID_neutralSum, **jetID_chargedSum, **jetID_eSum;
		TH2D **jetEnergyPt, **jetEnergyEta ; //for studying JEC and JER
		TH3D **JEC3D;
		TH1D **jecptMu , **jecetaMu, **jecptSigma, **jecetaSigma; 
		TProfile2D **jec_eta_phi_mu;
		TString caption = "";
};


template <typename event, typename config>
class producerJetQA : public producerBase<event,config>{
	public :
		producerJetQA(const char* name):producerBase<event, config>(name){
		}
		~producerJetQA(){}

		bool linkFrame(liteFrame<event, config> *frame){frame->doJet= 1; 
			//frame->evt->addJetID = doJetID; 
			return 0;}
		void beginJob(){
			this->evt->loadJet(this->_cfg->ps->jetSetName);
		}
		void run(){
			int jcent = this->_frame->getCentIndex();
			if(jcent<0) return;
			//float weight = this->getEvtWeight();
			float weight = this->evtWeight;
			//if(weight!=1) cout<<"weight: "<<weight<<endl;
			//xTagger evtTg = this->_cfg->src->evtTag(this->evt);
			for(auto & it : jetSets){
				//if(! evtTg.select(it->evtMask)) continue;
				it->template fillEvtInfo<event>(this->evt, jcent,  weight);
			}
			//cout<<this->evt->nJet()<<endl;
			for(int i=0; i<this->evt->nJet(); ++i){
				xTagger tag = this->_cfg->src->tagRecoJet(this->evt, i);
				if(tag.tag == 0) continue;
				for(auto & it : jetSets){
					it->template fillHist<event>(tag, this->evt, jcent, i, weight);
				}
			}
			if(!(this->_cfg->ps->isMC)) return;
			for(int i=0; i<this->evt->nGenJet(); ++i){
				xTagger tag = this->_cfg->src->tagGenJet(this->evt, i);
				if(tag.tag == 0) continue;
				int index = -1;
				float drmin=1;
				for(int j=0; j<this->evt->nJet(); ++j){
					float dr = toolkit::findDr(this->evt->genjeteta[i],this->evt->genjetphi[i],this->evt->jeteta[j],this->evt->jetphi[j]);
					if(dr > 0.4) continue;
					if(dr < drmin ) {
						drmin = dr; index = j;
					}
				}

				for(auto & it : jetSets){
					if(index>-1) it->template fillJEC<event>(tag, this->evt,jcent, i, index, weight, jeccorr);
					//if(index>-1) it->template fillJEC<event>(tag, this->evt,jcent, i, index, weight, this->_cfg->src->jeutool);
				}
			}

		}

		void endJob(){
			//wf = TFile::Open(output,"recreate");
			//this->hm->write(wf);
		}

		jetQASet* addJetSet(TString name, int bit){
			auto js = new jetQASet(name, bit, this->_cfg->ps->ncent, this->_cfg->ps->centbin);
			js->init();
			jetSets.emplace_back(js);
			return js;
		}

		xTagger (*jetSelection)(event* em, int j)=0;

		std::vector<jetQASet*> jetSets;
		bool doJetID=0;

		JetCorrector jeccorr;	
};


#endif
