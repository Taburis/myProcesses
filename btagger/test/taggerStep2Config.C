
#include "myProcesses/btagger/plugin/bTaggerStep2Analyzer.h"
#include "myProcesses/jtc/plugin/config_aa2018_bjet.h"

void taggerStep2Config(){
	//auto fMC = TFile::Open("./data/btagger_step1_dijetSamples_shifted.root");
	auto fMC = TFile::Open("./data/btagger_step1_dijetSamples_reweighted.root");
	//auto fMC = TFile::Open("AA2018bTagger_dijetMC_HP_step1.root");
	auto fData = TFile::Open("./data/AA2018bTagger_DataJet80_HardprobDB_step1.root");
	using namespace AA2018bJet;
	TString outputfolder = "/eos/user/w/wangx/www/btagger/";
	
	auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_Reweighted");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_20Jan2020_Cent5Shifted");
	btagger->cent = new centralityHelper(ncent, centbins);
	btagger->folderPath = outputfolder;
	btagger->loadMC(fMC);
	btagger->loadData(fData);
cout<<"file loaded"<<endl;
	btagger->produceTH2QA();
	//btagger-> eventWeightCheck();
//	btagger->produceList();

}
