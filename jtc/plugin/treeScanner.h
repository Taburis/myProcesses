

#ifndef TREESCANNER_H
#define TREESCANNER_H
//#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#include "myProcesses/jtc/plugin/histManager.h"

class treeScanner;
class eventMap;

class scanPlugin {
	public:
		scanPlugin(){};
		virtual void run() = 0;
		virtual void endJob()=0;
		virtual void beginJob()=0;
		void linkEventContent(eventMap *em0){
			em = em0;
		}
		float getEvtWeight();
		eventMap *em=nullptr;
		treeScanner *ts;
		histManager *hm;
};

class treeScanner{
	public:
		treeScanner(eventMap* em0){
			em = em0;
			hm = new histManager();
		}
		TFile* _file=nullptr;

		void addPlugin(scanPlugin* sp){
			sp->linkEventContent(em);
			//sp->em = em;
			sp->ts = this;
			//sp->hm = hm;
			plugins.push_back(sp);
		}
		bool noCut(eventMap* em){return 0;}// always keep evt, for convenient reason

		bool basicEvtCuts(eventMap *em){
			if(em->checkEventFilter()) return 1;
			if(TMath::Abs(em->vz) > 15) return 1;
			return 0;
		}
		void run();
		float (*evtWeight)(eventMap *em) = nullptr;
		std::vector<scanPlugin*> plugins;
		bool (*evtCut)(eventMap*em) =nullptr;
		bool initCheck(){
			if( evtCut == nullptr ) return 1;
			return 0;
		}
		void write(){
			if(outf == nullptr) outf=TFile::Open(outputName, "recreate");
			else outf->cd();
			hm->write();
			outf->Close();
		}
		void loop();

		TFile *outf= nullptr;
		histManager *hm = nullptr;
		TString outputName = "output.root";
		eventMap *em = nullptr;
		Long64_t nevt = -1;
		bool reportPercent = 1;
		float evtW = 1;
};

float scanPlugin::getEvtWeight(){return ts->evtW;}

void treeScanner::run(){
	for(auto &it:plugins) it->beginJob();
	loop();
	for(auto &it:plugins) it->endJob();
}

void treeScanner::loop(){

	if(initCheck()){
		std::cout<<"Error: Initial checking failed!"<<std::endl;
		return;
	}
	Long64_t nentries = nevt < 0 ? em->evtTree->GetEntriesFast() : nevt;
	Long64_t nEvtPercent =floor(Double_t(nentries)/100);
	int npercent = 0;
	for(Long64_t jentry = 0; jentry< nentries; ++jentry){
		if(reportPercent && jentry % nEvtPercent == 0){
			std::cout<<"processed "<<npercent<<"\% events ... "<<std::endl;	
			npercent++;
		}
		else if(!reportPercent && jentry%1000 ==0 ) std::cout<<"processed "<<jentry<<" events ... "<<std::endl;
		em->getEvent(jentry);	
		if(evtCut(em)) continue;
		if(evtWeight !=nullptr) evtW = evtWeight(em);
		for(auto &it : plugins) it->run();
	}
}

#endif
