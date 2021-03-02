#include "math.h"
#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/QATools.h"
#include "myProcesses/jtc/plugin/jtcLib.h"
#include "myProcesses/jtc/plugin/producerJetQA.h"

#ifndef PRODUCER_DIJETQA_H
#define PRODUCER_DIJETQA_H

template <typename event, typename config>
class producerDijetQA : public producerBase<event,config>{
	public :
		producerDijetQA(const char* name):producerBase<event, config>(name){
		}
		~producerDijetQA(){}
		bool linkFrame(liteFrame<event, config> *frame){frame->doJet= 1; 
			return 0;}
		void beginJob(){
			this->evt->loadJet(this->_cfg->ps->jetSetName);
			incl = new jetQASet("inclusive", 0, this->_cfg->ps->ncent, this->_cfg->ps->centbin);
			incl->init();
			xj = incl->regHist<TH1D>("Dijet_xj", "", 40, 0, 1);
			loadJEC();
			gHM->sumw2();
		}

		void run(){
			int jcent = this->_frame->getCentIndex();
			if(jcent<0) return ;
			float weight = this->evtWeight;
			xTagger tag(0);
			double ptlead = 120, ptsub = 50;
			int jlead = -1, jsub = -1;	
			auto evt = this->evt;
			for(int i=0; i<this->evt->nJet(); ++i){
				if(fabs(this->evt->jeteta[i]) > 2) continue; 
				incl->template fillHist<event>(tag, this->evt, jcent, i, weight);
				double cpt = correctPt(evt, i);
				if(ptlead < cpt){
					ptsub = ptlead;
					ptlead = cpt;
					jsub = jlead;
					jlead = i;
				}else if(ptsub < cpt) {
					ptsub = cpt;
					jsub = i;
				}
			}

			if(jsub <0 || jlead<0) return;
			if(fabs(evt->jeteta[jlead]) > 1.6 || fabs(evt->jeteta[jsub])> 1.6) return ;
			double dphi = acos(cos(evt->jetphi[jlead]-evt->jetphi[jsub])) ;
			if(dphi < 5.0/6*TMath::Pi()) return;
			//cout<<jlead<<"  :  "<<jsub<<endl;
			xj[jcent] ->Fill(ptsub/ptlead,weight);
			//cout<<ptsub/ptlead<<endl;
		}

		double correctPt(event *evt, int ijet){
			JEC.SetJetPT (evt->jetpt[ijet]);
			JEC.SetJetEta(evt->jeteta[ijet]);
			JEC.SetJetPhi(evt->jetphi[ijet]);
			return JEC.GetCorrectedPT();
		}
		void loadJEC(){
			jecFiles.emplace_back("../../HIN-20-003/JEC2018PbPb/Autumn18_HI_V6_DATA_L2Relative_AK4PF.txt");
			jecFiles.emplace_back("../../HIN-20-003/JEC2018PbPb/Autumn18_HI_V6_DATA_L2L3Residual_AK4PF.txt");
			JEC.Initialize(jecFiles);
		}
		void endJob(){
		}

		jetQASet *incl;
		JetCorrector JEC;
		std::vector<string> jecFiles;
		TH1D** xj;
};

#endif
