
#include "myProcesses/btagger/plugin/bTaggerStep2Analyzer.h"
#include "myProcesses/jtc/plugin/config_aa2018_bjet.h"
using namespace AA2018bJet;

void taggerStep2Config(){
	config_init();
	//auto fMC = TFile::Open("./data/btagger_step1_dijetSamples_shifted.root");
	auto fMC = TFile::Open("data/btagger_step1_dijetSamples_pthat60_evtWeighted_5centShifted.root");
	//auto fMC = TFile::Open("data/btagger_step1_dijetSamples_pthat60_evtWeighted.root");
	auto fData = TFile::Open("data/AA2018bTagger_DataJet80_HardprobDB_step1.root");
	TString outputfolder = "plot/";
	//TString outputfolder = "/eos/user/w/wangx/www/btagger/";
	
	auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_5centShift");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_20Jan2020_Cent5Shifted");
	auto chp = new centralityHelper(ncent, centbins);
	btagger->linkCentralityHelper(chp); 
	btagger->folderPath = outputfolder;
	btagger->_mcf = fMC;
	btagger->_dataf = fData;
	btagger->loadMC(fMC);
	btagger->loadData(fData);

	//btagger->produceTH2QA();
	btagger->calculateSF(3, 120, 499);
	//btagger->calculateSF(3, 120, 499);
	//btagger->JEC("csv0p9", "csv0p9/");
//	TDirectory *dir = (TDirectory *) fMC->Get("csv0p9");
//	btagger->srmc.load_TH1_from_dir(dir);
//	btagger->srmc["csv0p9/csv0p9_jec_b_C0"]->Draw();
//	btagger->scaleFactorPlot("csvSF.png", "csvJetPtBins", 4, 4);
	//btagger-> eventWeightCheck();
//	btagger->produceList();

}
