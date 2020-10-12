
#ifndef ANALYZER_BTAGGING_H
#define ANALYZER_BTAGGING_H
#include "myProcesses/liteFrame/plugin/analyzerBase.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/QATools.h"

template <typename config>
class analyzerBTagging : public analyzerBase<config>{
	public :
		analyzerBTagging(config & cfg):analyzerBase<config>(cfg){
		}
		~analyzerBTagging(){}

		void loadHist(TString path, bool ismc = 0){
			auto f = TFile::Open(path);
			if(ismc) fmc=f;
			else fdata=f;
			int n = this->_cfg->ps->ncent;
			if(isEmpty){
				qaPt= new qaTSetFlavor<TH1D>("jetpt", this->_cfg->ps->ncent, f, ismc);
				qaEta= new qaTSetFlavor<TH1D>("jeteta", this->_cfg->ps->ncent, f, ismc);
				qaPhi= new qaTSetFlavor<TH1D>("jetphi", this->_cfg->ps->ncent, f, ismc);
				qaTagger= new qaTSetFlavor<TH1D>("tagger", this->_cfg->ps->ncent, f, ismc);
				qaNTagger= new qaTSetFlavor<TH1D>("nTagger", this->_cfg->ps->ncent, f, ismc);
			}else {
				qaPt->load("jetpt", n, f, ismc);
				qaEta->load("jeteta", n, f, ismc);
				qaPhi->load("jetphi", n, f, ismc);
				qaTagger->load("tagger", n, f, ismc);
				qaNTagger->load("nTagger", n, f, ismc);
			}
			isEmpty = 0;
		}

		void makeStackPlot(TString name, TString path, qaTSetFlavor<TH1D> *set, bool doLogy = 0, TString xtitle = "X", TString ytitle = ""){
			auto cm = new multi_pads<stack_pad>("c_"+name, "", 1, set->ncent);
			cm->xtitle = xtitle;
			cm->ytitle = ytitle;
			for(int i=0; i<set->ncent; i++){
				cm->at(0,set->ncent-i-1)->sp= new stackPlot("stockPad_"+name+Form("_%d",i));
				stackPlot * sp = cm->at(0,set->ncent-i-1)->sp;
				sp->addLegend();
				cm->at(0,set->ncent-1-i)->doNorm=1;
				cm->at(0,set->ncent-1-i)->doLogy=doLogy;
				if(set->onMC){
					auto hb = set->hist_b[i];
					hb->SetFillColor(kViolet-5);
					hb->SetFillStyle(1001);
					auto hc = set->hist_c[i];
					hc->SetFillColor(kOrange+7);
					hc->SetFillStyle(1001);
					auto hl = set->hist_l[i];
					hl->SetFillColor(kBlue-9);
					hl->SetFillStyle(1001);
					sp->add(hl, "udsg", "f");
					sp->add(hc, "c", "f");
					sp->add(hb, "b", "f");
				}
				if(set->onData) {
					auto hd = set->hist_data[i];
					cm->at(0,set->ncent-1-i)->addReference(hd, "data");
				}
			}
			cm->at(0,1)->doLegend = 1;
			cm->draw();
			for(int i=0; i<set->ncent; i++){
				cm->at(0,set->ncent-1-i)->addTextUpPad(0.25,0.85, this->_cfg->ps->centLabel[i]);
			}
			cm->SaveAs(path+"/stackPlot_"+name+".png");
		}

		void analyze(){
			const int dir_fig = system("mkdir -p "+output+"/bTaggingQAs/");	
			TString path = output+"/bTaggingQAs";
			makeStackPlot("jetpt", path, qaPt, 1, "p_{T}^{jet}");
			makeStackPlot("jeteta", path, qaEta, 1, "#eta^{jet}");
			makeStackPlot("jetphi", path, qaPhi, 1, "#phi^{jet}");
			makeStackPlot("bTagger", path, qaTagger, 1, "csv-v2 value");
			makeStackPlot("negBTagger", path, qaNTagger, 1, "csv-v2 value (negative)");
		}

		TFile *fmc, *fdata;
		TString output;
		qaTSetFlavor<TH1D> *qaPt, *qaEta, *qaPhi, *qaTagger, *qaNTagger;
		bool isEmpty = 1;
};

#endif
