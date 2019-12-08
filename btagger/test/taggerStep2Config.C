
#include "bTaggerStep2Analyzer.h"
#include "config_aa2018_bjet.h"

void taggerStep2Config(){
	auto fMC = TFile::Open("../macro/AA2018bTagger_dijetMC_HP_step1.root");
	auto fData = TFile::Open("../macro/AA2018bTagger_DataJet80_HardprobDB_step1.root");
	using namespace AA2018bJet;
	
	auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger");
	btagger->cent = new centralityHelper(ncent, centbins);
	btagger->loadMC(fMC);
	btagger->loadData(fData);

	btagger->produceTH2QA();
//	btagger->produceList();

}
