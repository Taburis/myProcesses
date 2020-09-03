
#ifndef scanPlugin_jetReco_H
#define scanPlugin_jetReco_H

#include "myProcesses/jtc/plugin/simpleScanner.h"
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
		void fillJER(TH2D* h, int jreco, int jgen, float w = 1){
			float genpt = this->evt->genjetpt[jgen];
			float recpt = this->evt->jetpt[jgen];
			h->Fill(genpt, recpt/genpt, w);
		}


		void run();
		void endJob(){}
		void beginJob(){
			this->ss->doJet= 1;
			jer = hist_JER("inclusive");
			jer_b = hist_JER("b");
			jer_tag = hist_JER("tagged");
		}
		TH2D** jec, **jec_b, **jec_tag, **jer, **jer_b, **jer_tag;

		TString _name = "jetRecoQA";
		float jetptmin = 100, jetptmax = 500;
		int nptbin = 200;
};

template <typename cfg, typename event>
void scanPlugin_jetReco<cfg, event>::run(){
	int jcent = this->ss->getCentIndex();
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
		fillJER(jer[jcent], i, jgen, weight);
		if(fabs(this->evt->flavor_forb[i])==5) fillJER(jer_b[jcent], i, jgen, weight);
		if(this->evt->disc_csvV2[i]>0.9) fillJER(jer_tag[jcent], i, jgen, weight);
	}
}

#endif
