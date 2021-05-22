
#ifndef liteFrame_H
#define liteFrame_H

#include "TRandom.h"
#include "TBenchmark.h"
#include "myProcesses/liteFrame/plugin/histManager.h"
#include "myProcesses/liteFrame/plugin/xAxis.h"
#include "myProcesses/liteFrame/plugin/xTagger.h"

template <typename event, typename config>
class liteFrame;

histManager * gHM = new histManager();

template <typename PSet, typename Selection, typename Weight>
class configBase{
	public :
		configBase(){
			ps = new PSet();
			src  = new Selection();
			weight  = new Weight();
		}
		~configBase(){}

		PSet *ps;
		Selection *src;
		Weight *weight;
};


template <typename event, typename config>
class producerBase {
	public:
		producerBase(const char * name): producerName(name){};
		~producerBase(){};
		virtual void run() =0;
		virtual void endJob()=0;
		virtual void beginJob()=0;
		virtual bool linkFrame(liteFrame<event, config> *frame)= 0;
		float getEvtWeight ();

		liteFrame<event, config> *_frame;
		event* evt;
		config * _cfg;
		histManager *hm;
		int jcent=0;
		float evtWeight = 1;
		std::string producerName;
};

template <typename event, typename config>
class liteFrame{
	public : 
		liteFrame(){}
		liteFrame(TString name0, config &cfg, TFile * f)
		{	_f = f; 
			_cfg = &cfg;
			name = name0;
			//hm = new histManager();
			hm = gHM;
			evt = new event(f);
			output = name0+"_output.root";
		}
		~liteFrame(){}
		void init(TFile *f);
		void begin();
		void end();
		void run();
		void loop();
		void addProducer(producerBase<event, config>* sp){
			sp->evt = evt;
			sp->_frame = this;
			sp->_cfg = _cfg;
			sp->hm = gHM;
			if(sp->linkFrame(this)){
				std::cout<<"Producer: "<<sp->producerName<<" failed at the linking stage"<<std::endl;
				return;
			};
			producers.emplace_back(sp);
		}
		int getCentIndex(){
			if(!isHI) return 0;	
			return ax->find_bin_in_range(evt->hiBin);
		}

		config* _cfg; event* evt; TFile * _f, *_wf;
		float evtWeight=1;
		TString name, output;
		bool isMC, isHI;
		bool doTrk=0, doJet=0, doGenJet=0, doGenParticle=0;
		histManager *hm;
		std::vector<producerBase<event, config>*> producers;
		Long64_t nevt = -1, evt_start = 0;
		xAxis *ax;
		int jcent;
};

template <typename event, typename config>
float producerBase<event, config>::getEvtWeight() {return _frame->evtWeight;}

template <typename event, typename config>
void liteFrame<event, config>::init(TFile* f){
	evt->isMC=_cfg->ps->isMC;
	evt->isHI=_cfg->ps->isHI;
	isMC=_cfg->ps->isMC;
	isHI=_cfg->ps->isHI;
	evt->init();
	int nfilters = _cfg->ps->nfilters;
	int ntrigs = _cfg->ps->ntrigs;
	if(ntrigs !=0) evt->loadTriggerInfo(ntrigs, _cfg->ps->trigger);
	if(nfilters !=0) evt->regEventFilter(nfilters, _cfg->ps->evtFilterString);
	if(doTrk) evt->loadTrack(); 
	if(doGenParticle) evt->loadGenParticle(); 
	if(doJet) evt->loadJet(_cfg->ps->jetSetName); 
	if(!isHI) return;
	ax = new xAxis(_cfg->ps->ncent, _cfg->ps->centbin);
}

template <typename event, typename config>
void liteFrame<event, config>::run(){

	std::cout<<"checking jobs..."<<std::endl;
	bool dostop = 0;

	if(dostop) return;

	_wf = TFile::Open(output,"recreate");
	init(_f);
	cout<<"initialized event"<<endl;
	for(auto &it:producers) it->beginJob();
	std::cout<<"initializing..."<<std::endl;
	hm->sumw2();
	std::cout<<"starting event loop..."<<std::endl;
	loop();
	std::cout<<"finalizing jobs..."<<std::endl;
	for(auto &it:producers) it->endJob();
	_wf->cd();
	std::cout<<"saving output..."<<std::endl;
	hm->write(_wf);
	_wf->Close();
	std::cout<<"process finished!!"<<std::endl;
}

template <typename event, typename config>
void liteFrame<event, config>::loop(){
	TBenchmark clock; clock.Start("loop");

	Long64_t nentries = nevt < 0 ? evt->evtTree->GetEntriesFast() : nevt;
	cout<<"total events: "<<nentries<<endl;
	Long64_t nEvtPercent =floor(float(nentries)/100);
	if( nEvtPercent == 0) nEvtPercent = 1;
	int npercent = 0;
	for(Long64_t jentry = evt_start; jentry< nentries; ++jentry){
		if( jentry % nEvtPercent == 0){
			std::cout<<"processed "<<npercent<<"\% events ... "<<std::endl;	
			npercent++;
		}
		//if(jentry%1000 ==0 ){
		//	std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
		//	clock.Show("loop");
		//	clock.Reset();
		//	clock.Start("loop");
		//}
		evt->getEvent(jentry);	
		if(_cfg->src->evtCut(evt)) continue;
		jcent =  getCentIndex();
		if(jcent < 0) continue;

		if(isMC)  evtWeight = _cfg->weight->evtWeight(evt);

		for(auto &it : producers){
			it->evtWeight = evtWeight;
			it->jcent = jcent;
			it->run();
		}
	}
}

#endif
