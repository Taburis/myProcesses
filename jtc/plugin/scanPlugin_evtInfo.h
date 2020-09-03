
#ifndef scanPlugin_evtInfo_H
#define scanPlugin_evtInfo_H
#include "myProcesses/jtc/plugin/simpleScanner.h"
#include "myProcesses/jtc/plugin/toolkit.h"
template <typename cfg, typename event>
class scanPlugin_evtInfo : public scanPlugin<cfg,event>{
	public :
		scanPlugin_evtInfo(){}
		~scanPlugin_evtInfo(){}
		void endJob(){};
		void beginJob(){
			if(this->_cfg->isHI){
				hcent= this->hm->template regHist<TH1D>("hcent", "hiBin distribution", 200, 0, 200);
	doCent = 1;
			if(this->_cfg->isMC){
				doPthat = 1;
				hpthat = this->hm->template regHist<TH1D>("hpthat", "pThat distribution", 400, 0, 400);
}
}
			hvz = this->hm->template regHist<TH1D>("hvz", "Vz distribution", 200, -20, 20);
		}
		void run(){
			float w = this->ss->evtWeight;
			if(doCent) hcent->Fill(this->evt->hiBin,w);
			if(doPthat)hpthat->Fill(this->evt->pthat,w);
			hvz->Fill(this->evt->vz, w);
		}

		TH1D* hvz, *hcent, *hpthat;
		bool doCent=0, doPthat=0;
};

#endif
