
#include "myProcesses/btagger/plugin/bTaggerStep2Analyzer.h"
#include "myProcesses/jtc/plugin/config_aa2018_bjet.h"

void taggerStep2Config(){
	auto fMC = TFile::Open("AA2018bTagger_dijetMC_HP_step1.root");
	auto fData = TFile::Open("AA2018bTagger_DataJet80_HardprobDB_step1.root");
	using namespace AA2018bJet;
	TString outputfolder = "/eos/user/w/wangx/www/btagger/";
	
	auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_11Dec2019");
	btagger->cent = new centralityHelper(ncent, centbins);
	btagger->folderPath = outputfolder;
	btagger->loadMC(fMC);
	btagger->loadData(fData);

	btagger->produceTH2QA();
//	btagger->produceList();

}
