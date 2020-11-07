
#ifndef ANALYZER_BTAGGING_H
#define ANALYZER_BTAGGING_H
#include "myProcesses/liteFrame/plugin/analyzerBase.h"
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/producerBTagging.h"
#include "myProcesses/jtc/plugin/QATools.h"

template <typename config>
class analyzerBTagging : public analyzerBase<config>{
	public :
		analyzerBTagging(config & cfg):analyzerBase<config>(cfg){
		}
		~analyzerBTagging(){}

		void bJetType(){
			bJetSet = new xTSetBase("bTaggingSet", this->_cfg->ps->ncent);	
			gsppt  = bJetSet->loadHist<TH1D>("gspPt", fmc);
			gspeta = bJetSet->loadHist<TH1D>("gspEta", fmc);
			gspphi = bJetSet->loadHist<TH1D>("gspPhi", fmc);
			bjetpt = bJetSet->loadHist<TH1D>("bjetPt", fmc);
			bjeteta= bJetSet->loadHist<TH1D>("bjetEta", fmc);
			bjetphi= bJetSet->loadHist<TH1D>("bjetPhi", fmc);
	
			plot_overlay("bJetType_Pt", path,bjetpt,"incl. b jet", gsppt, "GSP. b jet", "p_{T}^{jet} (GeV)", "A.U. (inclusive b jets)");
		}
	
		void plot_overlay(TString name, TString path0, TH1D** h1, TString lab1, TH1D** h2, TString lab2, TString xtitle, TString ytitle, bool dology = 0){
			int nc = this->_cfg->ps->ncent;
			auto c = new plotManager();
			c->initOverlayPad("canvas_"+name, "", 1, nc);
			for(int i=0;i<nc; ++i){
				h1[i]->GetXaxis()->SetTitle(xtitle);
				h1[i]->GetYaxis()->SetTitle(ytitle);
				c->addHist(h1[i], 0, nc-1-i, lab1);
				c->addHist(h2[i], 0, nc-1-i, lab2);
				((overlayPad*)c->at(0, nc-1-i))->rymin=0;
				((overlayPad*)c->at(0, nc-1-i))->rymax=1.1;
			}
			c->setXrange(qaconvention::ptbin[0],qaconvention::ptbin[20]-0.1);
			c->draw();
			c->drawLegend();
			for(int i=0;i<nc; ++i){
				float frac = h2[i]->Integral()/h1[i]->Integral();
				c->at(0, nc-1-i)->drawText(0.5, 0.6,Form("frac:%f",frac));
			}
			c->save(path+"/"+name+".png");
		}

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
				pt_all= new qaTSetFlavor<TH1D>("jetpt_all", this->_cfg->ps->ncent, f, ismc);
				pt_neg= new qaTSetFlavor<TH1D>("jetpt_negTag", this->_cfg->ps->ncent, f, ismc);
			}else {
				qaPt->load("jetpt", n, f, ismc);
				qaEta->load("jeteta", n, f, ismc);
				qaPhi->load("jetphi", n, f, ismc);
				qaTagger->load("tagger", n, f, ismc);
				qaNTagger->load("nTagger", n, f, ismc);
				pt_all->load("jetpt_all", n, f, ismc);
				pt_neg->load("jetpt_negTag", n, f, ismc);
			}
			if(ismc){
				pt_all->summation();
				pt_neg->summation();
			}
			isEmpty = 0;
		}

		void getScaleFactor(TString path){
			int nc = this->_cfg->ps->ncent;
			hscale = new TH1D*[nc];
			auto haux = new TH1D*[nc];
			scaleF = new float[nc];
			for(int i=0; i<nc; i++){
				scaleF[i]= pt_neg->hist_data[i]->Integral();
				scaleF[i]= scaleF[i]/pt_all->hist_data[i]->Integral();
				scaleF[i]= scaleF[i]*(pt_all->hist_mc[i]->Integral());
				scaleF[i]= scaleF[i]/(pt_neg->hist_mc[i]->Integral());

				hscale[i]=(TH1D*) pt_neg->hist_data[i]->Clone(Form("hscale_C%d",i));
//				hscale[i]->Divide(hscale[i],pt_all->hist_data[i],1, 1,"B");
				haux[i]=(TH1D*) pt_neg->hist_mc[i]->Clone(Form("hscale_aux_C%d",i));
				haux[i]->Divide(hscale[i],pt_all->hist_mc[i],1, 1,"B");
				//hscale[i]->Divide(haux[i]);
			}
			auto c = new plotManager();
			c->initSquarePad("canvas_scaleF", "", 1, nc);
			for(int i=0; i<nc; i++){
				//c->addHist(haux[i], 0, nc-1-i);
				//c->addHist(hscale[i], 0, nc-1-i);
				c->addHist(pt_all->hist_data[i], 0, nc-1-i);
				c->addHist(pt_neg->hist_data[i], 0, nc-1-i);
			}
//			c->setYrange(0., 2);
			c->setXrange(qaconvention::neg_ptbin[0], qaconvention::neg_ptbin[5]-1);
			c->draw();
			for(int i=0; i<nc; i++){
				c->at(0,nc-1-i)->drawText(0.25, 0.8, Form("Integrated SF:%f",scaleF[i]));
			}
			c->save(path+"/scaleFactor.png");
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
			path = output+"/bTaggingQAs";
			getScaleFactor(path);
			bJetType();
			//makeStackPlot("jetpt", path, qaPt, 1, "p_{T}^{jet}");
			//makeStackPlot("jeteta", path, qaEta, 1, "#eta^{jet}");
			//makeStackPlot("jetphi", path, qaPhi, 1, "#phi^{jet}");
			//makeStackPlot("bTagger", path, qaTagger, 1, "csv-v2 value");
			//makeStackPlot("negBTagger", path, qaNTagger, 1, "csv-v2 value (negative)");
		}

		TFile *fmc, *fdata;
		TString output, path;
		qaTSetFlavor<TH1D> *qaPt, *qaEta, *qaPhi, *qaTagger, *qaNTagger;
		qaTSetFlavor<TH1D> *pt_neg, *pt_all;
		TH1D** hscale;
		TH1D** gsppt, **gspeta, **gspphi, **bjetpt, **bjeteta, **bjetphi;
		float *scaleF;
		xTSetBase *bJetSet;
		bool isEmpty = 1;
};

#endif
