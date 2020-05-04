
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#include "myProcesses/btagger/plugin/bTaggerStep2Analyzer.h"
#include "myProcesses/jtc/config/config_aa2018_bjet.h"
using namespace AA2018bJet;

void taggerStep2Config(){
	config_init();
	//auto fMC = TFile::Open("data/btagger_QAs_dijetMC_pthat60.root");
	auto fMC = TFile::Open("AA2018bTagger_id1.root");
	//auto fMC = TFile::Open("data/btagger_QAs_dijetMC_pthat60_jteta2.root");
	//auto fMC = TFile::Open("data/btagger_QAs_dijetMC_pthat60_JESJER.root");
	//auto fMC = TFile::Open("data/btagger_QAs_dijetMC_pthat60_jtptWeighted.root");
	//auto fData = TFile::Open("data/btagger_QAs_data_jet80trig_shift5centOut.root");
	auto fData = TFile::Open("data/btagger_QAs_data_jet80trig_jeteta2.root");
	//auto fData = TFile::Open("data/btagger_QAs_data_jet80trig.root");
//	TString outputfolder = "plot/";
	TString outputfolder = "/eos/user/w/wangx/www/btagger/";
	
	//auto btagger  = new bTaggerStep2Analyzer("test");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger");
	auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_muonRiched");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_MCptWeighted_forbRAA");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_MCptWeighted");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_MCptWeighted_JESJER");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_data5centShiftOut");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_20Jan2020_Cent5Shifted");
	auto chp = new centralityHelper(ncent, centbins);
	btagger->linkCentralityHelper(chp); 
	btagger->folderPath = outputfolder;
	btagger->_mcf = fMC;
	btagger->_dataf = fData;
	btagger->init();
	btagger->loadMC(fMC);
	btagger->loadData(fData);

	//btagger->drawQAs();
	btagger->tagBias();
	//btagger->produceTH2QA();
	//btagger->calculateSF_Data(3, 120, 499);
	//btagger->calculateSF(3, 120, 499);
	//btagger->calculateSF(3, 120, 499);
	btagger->JEC("csv0p9", "csv0p9/");

}
