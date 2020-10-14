
#ifndef ANALYZER_BTAGGING_H
#define ANALYZER_BTAGGING_H
#include "myProcesses/liteFrame/plugin/analyzerBase.h"
#include "myProcesses/jtc/plugin/producerJetQA.h"
#include "myProcesses/jtc/plugin/plotLib.h"

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

		void qaPlot_jetID(TString path){
			auto ncent = this->_cfg->ps->ncent;
			auto mp = new multi_pads<overlay_pad>("canvas_jetID", "", 7, ncent);
			mp->addLegend("upperright");
			int index = 0;
			for(auto & it : jetSets){
				it->selfNormalization();
				for(int i=0; i<ncent; ++i){
					mp->add((TH1*) it->jetID_trkSum    [i], 0, ncent-1-i);
					mp->add((TH1*) it->jetID_hcalSum   [i], 1, ncent-1-i);
					mp->add((TH1*) it->jetID_ecalSum   [i], 2, ncent-1-i);
					mp->add((TH1*) it->jetID_photonSum [i], 3, ncent-1-i);
					mp->add((TH1*) it->jetID_neutralSum[i], 4, ncent-1-i);
					mp->add((TH1*) it->jetID_chargedSum[i], 5, ncent-1-i);
					mp->add((TH1*) it->jetID_eSum      [i], 6, ncent-1-i);
				}
				mp->labelHist(it->_name, index);
				index++;
			}
			mp->draw();
			mp->SaveAs(path+"/jetQA_jetID.png");
		}

		void analyze(){
			const int dir_fig = system("mkdir -p "+output+"/jetQAPlot/");
                        TString path = output+"/jetQAPlot";
			if(doJetID) qaPlot_jetID(path);
		}

		std::vector<jetQASet*> jetSets;
		bool doJetID=0;
		TString output;
};

#endif
