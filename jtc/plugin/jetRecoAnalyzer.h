
#ifndef jetRecoAnalyzer
#define jetRecoAnalyzer

#include "myProcesses/jtc/plugin/treeScanner.h"

class jetRecoAnalyzer: public scanPlugin{
	public : 
		jetRecoAnalyzer(){}
		virtual void beginJob();
		virtual void endJob();
		virtual void run();
	// for Y bin: 0 inclusive jet, 1 tagged jet
		TH1D **hrandmCone;
		centralityHelper *cent=nullptr;
};

void jetRecoAnalyzer::beginJob(){
	em->loadJet(js_name);
	hm = new histManager();
	int ncent = cent->nbins;
	for(int i=0; i<ncent; ++i){
		TString centl  = cent->centLabel[i];
	}
}
#endif 
