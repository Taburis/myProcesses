
#ifndef simpleScanner_H
#define simpleScanner_H

#include "myProcesses/jtc/plugin/histManager.h"
#include "myProcesses/jtc/plugin/xAxis.h"

template<typename cfg, typename event>
class simpleScanner;

template<typename cfg, typename event>
class scanPlugin {
	public:
		scanPlugin(){};
		~scanPlugin(){};
		virtual void run() =0;
		virtual void endJob()=0;
		virtual void beginJob()=0;
		float getEvtWeight () {return ss->evtWeight;}

		simpleScanner<cfg, event> *ss;
		event* evt;	
		histManager *hm;
		int ncent = 1;
		cfg* _cfg;
};

template<typename cfg, typename event>
class simpleScanner{
	public : 
		simpleScanner(){}
		simpleScanner(TString name0, cfg &cf, TFile * f){_f = f; _cfg=&cf;
			name = name0;
			hm = new histManager();
			evt = new event(f);
		}
		~simpleScanner(){}
		void init(TFile *f);
		void begin();
		void end();
		void run();
		void loop();
		void addPlugin(scanPlugin<cfg, event>* sp){
			sp->evt = evt;
			sp->ss = this;
			sp->hm = hm;
			plugins.emplace_back(sp);
			sp->_cfg = _cfg;
		}
		float (*eventW)(event*)=nullptr;
		float (*jetW)(event*, int)=nullptr;
		float (*trkW)(event*, int)=nullptr;
		int getCentIndex(){
			if(!isHI) return 1;	
			return ax->find_bin_in_range(evt->hiBin);
		}

		cfg* _cfg; event* evt; TFile * _f, *_wf;
		float evtWeight=1;
		TString name, output;
		bool isMC, isHI;
		bool doTrk=0, doJet=0, doGenJet=0, doGenParticle=0;
		histManager *hm;
		std::vector<scanPlugin<cfg, event>*> plugins;
		Long64_t nevt = -1;
		xAxis *ax;
};

template<typename cfg, typename event>
void simpleScanner<cfg,event>::init(TFile* f){
	evt->isMC=_cfg->isMC;
	evt->isHI=_cfg->isHI;
	evt->AASetup=_cfg->isHI;
	isMC = evt->isMC;
	isHI = evt->isHI;
	evt->init();
	if(doTrk) evt->loadTrack(); 
	if(doGenParticle) evt->loadGenParticle(); 
	if(doJet) evt->loadJet(_cfg->pset.jetSetName); 
	if(!isHI) return;
	ax = new xAxis(_cfg->pset.ncent, _cfg->pset.centbin);
}

template<typename cfg, typename event>
void simpleScanner<cfg,event>::run(){
	for(auto &it:plugins) it->beginJob();
	init(_f);
	hm->sumw2();
	loop();
	for(auto &it:plugins) it->endJob();
	_wf = TFile::Open(output,"recreate");
	_wf->cd();
	hm->write(_wf);
	_wf->Close();
}

template<typename cfg, typename event>
void simpleScanner<cfg,event>::loop(){
	Long64_t nentries = nevt < 0 ? evt->evtTree->GetEntriesFast() : nevt;
	cout<<"total events: "<<nentries<<endl;
	Long64_t nEvtPercent =floor(Double_t(nentries)/100);
	int npercent = 0;
	for(Long64_t jentry = 0; jentry< nentries; ++jentry){
		//		if(reportPercent && jentry % nEvtPercent == 0){
		//			std::cout<<"processed "<<npercent<<"\% events ... "<<std::endl;	
		//			npercent++;
		//		}
		//		else if(!reportPercent && jentry%1000 ==0 ) std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
		if(jentry%1000 ==0 ) std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
		evt->getEvent(jentry);	
		if(_cfg->cuts.evtCut(evt)) continue;
		if(isMC)  evtWeight = _cfg->weight.evtWeight(evt);

		for(auto &it : plugins) it->run();
	}
}

#endif
