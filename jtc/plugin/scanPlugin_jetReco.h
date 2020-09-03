
#ifndef scanPlugin_jetReco_H
#define scanPlugin_jetReco_H

#include "myProcesses/jtc/plugin/simpleScanner.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/toolkit.h"

template <typename cfg, typename event>
class scanPlugin_jetReco : public scanPlugin<cfg,event>{
	public : 
		scanPlugin_jetReco(){}
		~scanPlugin_jetReco(){}
		TH2D** hist_JER(TString tag){
			TH2D** h;
			h = new TH2D*[this->_cfg->pset.ncent];
			for(int i=0;i<this->_cfg->pset.ncent; ++i){
				h[i] = this->hm->template regHist<TH2D>(_name+"/JER_"+tag+Form("_%d",i),tag+" jet JEC: "+this->_cfg->pset.centLabel[i], nptbin, jetptmin, jetptmax, 200, 0, 2);
			}
			return h;
		}
		TH2D** loadHist(TString tag,  TFile *f){
			TH2D** h = new TH2D*[this->_cfg->pset.ncent];
			for(int i=0;i<this->_cfg->pset.ncent; i++){
				h[i]=(TH2D*)f->Get(_name+"/JER_"+tag+Form("_%d",i));
			}
			return h;
		}
		TH1D** produce_JEC(TString tag, TH2D** h2){
			auto h = new TH1D*[this->_cfg->pset.ncent];
			for(int i=0; i<this->_cfg->pset.ncent; i++){
				h[i]= h2[i]->ProfileX("JEC_"+tag+Form("_%d",i));
			}
			return h;	
		}
		void fillJER(TH2D* h, int jreco, int jgen, float w = 1){
			float genpt = this->evt->genjetpt[jgen];
			float recpt = this->evt->jetpt[jgen];
			h->Fill(genpt, recpt/genpt, w);
		}

		void produce(TFile *, TString);
		void run();
		void endJob(){}
		void beginJob(){
			this->ss->doJet= 1;
			jer2D = hist_JER("inclusive");
			jer2D_b = hist_JER("b");
			jer2D_tag = hist_JER("tagged");
		}
		TH2D   **jer2D, **jer2D_b, **jer2D_tag;
		TH1D   **jec, **jec_b, **jec_tag;
		TH1D   **jer, **jer_b, **jer_tag;

		TString _name = "jetRecoQA";
		float jetptmin = 100, jetptmax = 500;
		int nptbin = 200;
};

template <typename cfg, typename event>
void scanPlugin_jetReco<cfg, event>::run(){
	int jcent = this->ss->getCentIndex();
	if(jcent<0) return;
	float weight = this->getEvtWeight(); 
	for(int i=0; i<this->evt->nJet(); ++i){
		if(this->_cfg->cuts.recoJetCut(this->evt, i))continue;
		int jgen = -1;
		float dr0 = 0.4;
		for(int j=0; j<this->evt->nGenJet(); ++j){
			float dr = toolkit::findDr(this->evt->jeteta[i],this->evt->genjeteta[j], this->evt->jetphi[i],this->evt->genjetphi[j]);	
			if(dr0 < dr) continue;
			dr0=dr; jgen = j;
		}
		if(jgen< 0) continue;
		fillJER(jer2D[jcent], i, jgen, weight);
//cout<<"!"<<endl;
		if(fabs(this->evt->flavor_forb[i])==5) fillJER(jer2D_b[jcent], i, jgen, weight);
//cout<<"2"<<endl;
		if(this->evt->disc_csvV2[i]>0.9) fillJER(jer2D_tag[jcent], i, jgen, weight);
//cout<<"3"<<endl;
	}
}

template <typename cfg, typename event>
void scanPlugin_jetReco<cfg, event>::produce(TFile *f, TString path){
	jer2D = loadHist("inclusive",f);
	jer2D_b = loadHist("b",f);
	jer2D_tag = loadHist("tagged",f);
	jec = produce_JEC("inclusive", jer2D);
	jec_b = produce_JEC("b", jer2D_b);
	jec_tag = produce_JEC("tagged", jer2D_tag);
	auto c = new multi_pads<base_pad>("cJEC","", 1, this->_cfg->pset.ncent);
	for(int i=0; i<this->_cfg->pset.ncent; i++){
		c->cd(this->_cfg->pset.ncent-i);
		jec[i]->Draw("same");
		jec_b[i]->Draw("same");
		jec_tag[i]->Draw("same");
	}
	c->SaveAs(path+"/JEC_mu.png");
}

#endif
