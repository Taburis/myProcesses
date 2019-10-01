
#ifndef rootScanner_H 
#define rootScanner_H 


class rootAnalyzer {
	public :
		virtual int beginJob() = 0;
		virtual int endJob() = 0;
		virtual int analyze() = 0;
		TTree* handle(TString name);
};

class rootScanner {
	public :
		TFile* f;
		std::vector<rootAnalyzer*> sequence;	
		std::vector<TTree*> _forest;	
		rootScanner(TFile *f0);
		Long64_t maxEvent = 1;
		void addAnalyzer( rootAnalyzer *ana){
			sequence.push_back(ana);
		}
		TTree * handleTree(TString name){
			TTree *tree = (TTree*) f->Get(name);
			_forest.push_back(tree);
			return tree;
		}
		void getEntry(Long64_t i){
			for( auto it = _forest.begin(); it!=_forest.end(); ++it){
				(*it)->GetEntry(i);
			}
		}
		int run();
};

rootScanner *gFrame = 0;

TTree *rootAnalyzer::handle(TString name ){
	return gFrame->handleTree(name);
}

rootScanner::rootScanner(TFile *f0){
	f=f0;
	gFrame = this;
};

int rootScanner::run(){
	f->cd();
	gFrame = this;
	for( auto it = sequence.begin(); it!=sequence.end(); ++it){
		(*it)->beginJob();
	}
	if( maxEvent < 0)  maxEvent =  (_forest.at(0))->GetEntriesFast();
	std::cout<<"Starting the event loop for total "<<maxEvent<<" events... "<<std::endl;
	for(Long64_t ievt=0; ievt< maxEvent ; ++ievt){
		if(ievt % 10000 == 0) std::cout<<"processing "<<ievt<<"th event.."<<std::endl;
		getEntry(ievt);
		for( auto it = sequence.begin(); it!=sequence.end(); ++it){
			(*it)->analyze();
		}
	}

	for( auto it = sequence.begin(); it!=sequence.end(); ++it){
		(*it)->endJob();
	}
	return 0;
}

#endif
