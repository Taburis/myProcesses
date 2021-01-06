
#ifndef ANALYZER_JETQA_H
#define ANALYZER_JETQA_H
#include "myProcesses/liteFrame/plugin/analyzerBase.h"
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"
#include "myProcesses/liteFrame/plugin/Utility.h"
#include "myProcesses/jtc/plugin/producerJetQA.h"


template <typename config>
class analyzerJetQA : public analyzerBase<config>{
	public :
		analyzerJetQA(config & cfg):analyzerBase<config>(cfg){
		}
		~analyzerJetQA(){}

		void loadSet(TString name, TFile *f, TString label = ""){
			auto js = new jetQASet(name, 0, this->_cfg->ps->ncent, this->_cfg->ps->centbin);
			js->doJetID = doJetID;
			js->loadSet(name, f);
			jetSets.emplace_back(js);
			jetSetLabels.emplace_back(label);
		}

		void pinPointNormalize(float pin, TH1* h){
			int nb = h->GetXaxis()->FindBin(pin);
			float nn = h->GetBinContent(nb);
			for(int i=1; i<h->GetNbinsX()+1; ++i){
				float f = h->GetBinContent(i);
				h->SetBinContent(i, f/nn);
			}
		}

		void qaPlot_jetKinematics(TString path){
			auto ncent = this->_cfg->ps->ncent;
			auto mp = new plotManager();
			//			mp->initSquarePad("canvas_jetKinematics", "", 3, ncent);
			mp->initSquarePad("canvas_jetKinematics", "", 3, ncent);
			//mp->initOverlayPad("canvas_jetKinematics", "", 3, ncent);
			//			mp->addLegend("upperright");
			int index = 0;
			for(auto & it : jetSets){
				for(int i=0; i<ncent; ++i){
					if(pinPoint>0) pinPointNormalize(pinPoint, it->jetpt[i]);
					mp->addHist((TH1*) it->jetpt [i], 0, ncent-1-i,jetSetLabels[index],"pl");
					mp->addHist((TH1*) it->jeteta[i], 1, ncent-1-i,jetSetLabels[index],"pl");
					mp->addHist((TH1*) it->jetphi[i], 2, ncent-1-i,jetSetLabels[index],"pl");
					it->jetpt[i] -> GetXaxis()->SetTitle("p_{T}^{jet}");
					it->jeteta[i] -> GetXaxis()->SetTitle("#eta^{jet}");
					it->jetphi[i] -> GetXaxis()->SetTitle("#phi^{jet}");
				}
				//				mp->labelHist(it->_name, index);
				index++;
			}
			//			mp->doAutoYrange=1;
			mp->draw();
			mp->drawLegend("phase2");
			mp->c->SaveAs(path+"/jetQA_jetKinematics.png");
		}

		void qaPlot_JEC(TString path){
			auto ncent = this->_cfg->ps->ncent;
			auto mp = new plotManager();
			mp->initSquarePad("canvas_JEC", "", 2, ncent);
			int index = 0;
			for(auto & it : jetSets){
				it->produceJEC();
				for(int i=0; i<ncent; ++i){
					mp->addHist((TH1*) it->jecptMu [i], 0, ncent-1-i,jetSetLabels[index],"pl");
					mp->addHist((TH1*) it->jecetaMu [i],1, ncent-1-i,jetSetLabels[index],"pl");
					cout<<it->jecptMu[i]->GetName()<<endl;
					it->jecptMu[i] -> GetXaxis()->SetTitle("p_{T}^{Gen jet}");
					it->jecptMu[i] -> GetYaxis()->SetTitle("#mu_{p_{T}^{reco}/p_{T}^{gen}}");
					it->jecetaMu[i]-> GetXaxis()->SetTitle("#eta^{Gen jet}");
					it->jecetaMu[i] -> GetYaxis()->SetTitle("#mu_{p_{T}^{reco}/p_{T}^{gen}}");
					if(this->_cfg->ps->isHI)continue;
					it->jecetaMu[i]-> SetTitle("");
					it->jecptMu[i] -> SetTitle("");
					
				}
				//		mp->labelHist(it->_name, index);
				index++;
			}
			//		mp->doAutoYrange=1;
			mp->setYrange(0.9, 1.1);
			mp->draw();
			mp->drawHLine(1.0,2);
			mp->drawHLine(1.02,3);
			mp->drawHLine(.98,3);
			mp->drawLegend("upperright");
			mp->c->SaveAs(path+"/jetQA_JEC.png");

			auto mp_jer = new plotManager();
			mp_jer->initSquarePad("canvas_JER", "", 2, ncent);
			index = 0;
			for(auto & it : jetSets){
				for(int i=0; i<ncent; ++i){
					mp_jer->addHist((TH1*) it->jecptSigma [i], 0, ncent-1-i);
					mp_jer->addHist((TH1*) it->jecetaSigma [i],1, ncent-1-i);
					it->jecptSigma[i] -> GetXaxis()->SetTitle("p_{T}^{Gen jet}");
					it->jecptSigma[i] -> GetYaxis()->SetTitle("#sigma_{p_{T}^{reco}/p_{T}^{gen}}");
					it->jecetaSigma[i]-> GetXaxis()->SetTitle("#eta^{Gen jet}");
					it->jecetaSigma[i] -> GetYaxis()->SetTitle("#sigma_{p_{T}^{reco}/p_{T}^{gen}}");
					if(this->_cfg->ps->isHI)continue;
					it->jecetaSigma[i]-> SetTitle("");
					it->jecptSigma[i] -> SetTitle("");
					
				}
				index++;
			}
			mp_jer->setYrange(0., 0.3);
			mp_jer->draw();
			mp_jer->c->SaveAs(path+"/jetQA_JER.png");

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
			if(doJEC) qaPlot_JEC(path);
			if(doJetID) qaPlot_jetID(path);
		}

		std::vector<jetQASet*> jetSets;
		std::vector<TString> jetSetLabels;
		bool doJetID=0;
		bool doJEC= 0;
		float pinPoint = -1;
		TString output;
};

#endif
