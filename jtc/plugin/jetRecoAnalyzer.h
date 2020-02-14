
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
		TH1D **hjtCone;
		centralityHelper *cent=nullptr;
};

void jetRecoAnalyzer::beginJob(){
	em->loadJet(js_name);
	hm = new histManager();
	int ncent = cent->nbins;
	hrandmCone = new TH1D*[ncent];
	hjtCone = new TH1D*[ncent];
	for(int i=0; i<ncent; ++i){
		TString centl  = cent->centLabel[i];
	}
}
#endif 
