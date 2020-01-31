
#include "myProcesses/btagger/plugin/bTaggerStep2Analyzer.h"
#include "myProcesses/jtc/plugin/config_aa2018_bjet.h"

void taggerStep2Config(){
	//auto fMC = TFile::Open("./data/btagger_step1_dijetSamples_shifted.root");
	auto fMC = TFile::Open("btagger_step1_dijetSamples_pthat60_evtWeighted.root");
	auto fData = TFile::Open("AA2018bTagger_DataJet80_HardprobDB_step1.root");
	using namespace AA2018bJet;
	TString outputfolder = "plot/";
	//TString outputfolder = "/eos/user/w/wangx/www/btagger/";
	
	auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_Reweighted");
	//auto btagger  = new bTaggerStep2Analyzer("AA2018bTagger_20Jan2020_Cent5Shifted");
	auto chp = new centralityHelper(ncent, centbins);
	btagger->linkCentralityHelper(chp); 
	btagger->folderPath = outputfolder;
	btagger->loadMC(fMC);
	btagger->loadData(fData);

	double csvbin[] = {0, .8, .9};
	btagger->calculateSF(3, 120, 499);
	btagger->produceTH2QA();
	btagger->JEC("csv0p9", "csv0p9/");
//	TDirectory *dir = (TDirectory *) fMC->Get("csv0p9");
//	btagger->srmc.load_TH1_from_dir(dir);
//	btagger->srmc["csv0p9/csv0p9_jec_b_C0"]->Draw();
//	btagger->scaleFactorPlot("csvSF.png", "csvJetPtBins", 4, 4);
	//btagger-> eventWeightCheck();
//	btagger->produceList();

}
