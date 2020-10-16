
#ifndef ANALYZER_BTAGGING_H
#define ANALYZER_BTAGGING_H
#include "myProcesses/liteFrame/plugin/analyzerBase.h"
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"
#include "myProcesses/jtc/plugin/producerJetQA.h"

template <typename config>
class analyzerJetQA : public analyzerBase<config>{
	public :
		analyzerJetQA(config & cfg):analyzerBase<config>(cfg){
		}
		~analyzerJetQA(){}

		void loadSet(TString name, TFile *f){
			auto js = new jetQASet(name+"_jetQASet", 0, 0, this->_cfg->ps->ncent, this->_cfg->ps->centbin);
			js->doJetID = doJetID;
			js->loadSet(f);
			jetSets.emplace_back(js);
		}

		void qaPlot_jetKinematics(TString path){
			auto ncent = this->_cfg->ps->ncent;
			auto mp = new plotManager();
			mp->initSquarePad("canvas_jetKinematics", "", 3, ncent);
//			mp->addLegend("upperright");
			int index = 0;
			for(auto & it : jetSets){
				for(int i=0; i<ncent; ++i){
					mp->addHist((TH1*) it->jetpt [i], 0, ncent-1-i);
					mp->addHist((TH1*) it->jeteta[i], 1, ncent-1-i);
					mp->addHist((TH1*) it->jetphi[i], 2, ncent-1-i);
					it->jetpt[i] -> GetXaxis()->SetTitle("p_{T}^{jet}");
					it->jeteta[i] -> GetXaxis()->SetTitle("#eta^{jet}");
					it->jetphi[i] -> GetXaxis()->SetTitle("#phi^{jet}");
				}
//				mp->labelHist(it->_name, index);
				index++;
			}
//			mp->doAutoYrange=1;
			mp->draw();
			mp->c->SaveAs(path+"/jetQA_jetKinematics.png");
		}


		void qaPlot_jetID(TString path){
			auto ncent = this->_cfg->ps->ncent;
			auto mp = new plotManager();
			//mp->initSquarePad("canvas_jetID", "", 7, ncent);
			mp->initOverlayPad("canvas_jetID", "", 7, ncent);
			int index = 0;
			for(auto & it : jetSets){
				for(int i=0; i<ncent; ++i){
					mp->addHist((TH1*) it->jetID_trkSum    [i], 0, ncent-1-i);
					mp->addHist((TH1*) it->jetID_photonSum [i], 1, ncent-1-i);
					mp->addHist((TH1*) it->jetID_neutralSum[i], 2, ncent-1-i);
					mp->addHist((TH1*) it->jetID_chargedSum[i], 3, ncent-1-i);
					mp->addHist((TH1*) it->jetID_eSum      [i], 4, ncent-1-i);
					mp->addHist((TH1*) it->jetID_hcalSum   [i], 5, ncent-1-i);
					mp->addHist((TH1*) it->jetID_ecalSum   [i], 6, ncent-1-i);
					it->jetID_trkSum    [i]->GetXaxis()->SetTitle("trkSum");
					it->jetID_photonSum [i]->GetXaxis()->SetTitle("photonSum");
					it->jetID_neutralSum[i]->GetXaxis()->SetTitle("neutralSum");
					it->jetID_chargedSum[i]->GetXaxis()->SetTitle("chargedSum");
					it->jetID_eSum      [i]->GetXaxis()->SetTitle("eSum");
					it->jetID_hcalSum   [i]->GetXaxis()->SetTitle("hcalSum");
					it->jetID_ecalSum   [i]->GetXaxis()->SetTitle("ecalSum");
				}
//				mp->labelHist(it->_name, index);
				index++;
			}
//			mp->doAutoYrange=1;
			mp->draw();
			mp->c->SaveAs(path+"/jetQA_jetID.png");
		}

		void analyze(){
			for(auto & it : jetSets){
				it->selfNormalization();
			}
			const int dir_fig = system("mkdir -p "+output+"/jetQAPlot/");
                        TString path = output+"/jetQAPlot";
			qaPlot_jetKinematics(path);
			if(doJetID) qaPlot_jetID(path);
		}

		std::vector<jetQASet*> jetSets;
		bool doJetID=0;
		TString output;
};

#endif
