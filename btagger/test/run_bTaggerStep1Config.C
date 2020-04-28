
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#include "myProcesses/jtc/plugin/treeScanner.h"
#include "myProcesses/btagger/plugin/bTaggerAnalyzer.h"
#include "myProcesses/jtc/config/config_aa2018_bjet.h"
#include "myProcesses/jtc/plugin/Utility.h"

using namespace AA2018bJet;
TF1* fcentNew = new TF1("f","pol6", 0,200);
TH1D* hcsv_weight[4];
TH1D* hinclJtPtW[4];

float evtWeight0(eventMap* em){
	return fvzw.Eval(em->vz)*fcentNew->Eval(em->hiBin);
	//return 1;
	//float weight1 = inclJetEvtWeight(em);
	//return fw2->Eval(em->hiBin)*weight1;
}

bool evtCut1(eventMap *em){
	if(basicEvtCuts(em)) return 1;
	if(em->isMC) if(em->pthat < 60) return 1;
	return 0;
}
float csv_neg_weight(eventMap* em){
	return 1;
}

float jtptWf(eventMap* em, int i, int icent){
	int j = hinclJtPtW[icent]->FindBin(em->jetpt[i]);
	return hinclJtPtW[icent]->GetBinContent(j);
}
void run_bTaggerStep1Config(TString inf="/eos/cms/store/group/phys_heavyions/ikucher/bjetFrac/BJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_MuEnriched_Filter_isGSP_merged/BJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_MuEnriched_Filter_isGSP_part0.root", TString outf="AA2018bTagger_id1.root"){
	auto f = TFile::Open(inf);
// jt pt weight for MC to the data 5 cent shift out:
	auto fjtptw = TFile::Open("weight_inclJetPtWeight.root");
	for(int i=0; i<4; i++){
		hinclJtPtW[i] = (TH1D*) fjtptw->Get(Form("jtptW_smth_C%d",i));
	}
//-----------------------------------------

	bool doshift = 0;
	fcentNew->SetParameters(7.85523,-.289198,.00449837,-3.77752e-05,1.80269e-07,-4.62581e-10,4.97671e-13);	

	float centbins_5shift[] = {10, 30, 70, 110, 190};
	config_init();

	double jtptbins[5] = {120, 140, 180, 220, 500};
	double csvbins[3] = {0, 0.856,0.9};

	eventMap *em = new eventMap(f);
	em->isMC = 1;
	em->AASetup = 1;
	em->init();
	em->regEventFilter(nEvtFilter, evtFilters);
	treeScanner *ts = new treeScanner(em);
	ts->reportPercent = 0;

	auto btagger  = new bTaggerAnalyzer(outf);
	btagger->scheduleJetSet("akFlowPuCs4PFJetAnalyzer");
	btagger->configSF(4, jtptbins, 3, csvbins);
	if( doshift) {
		btagger->cent = new centralityHelper(ncent, centbins_5shift);
	}
	else {
		btagger->cent = new centralityHelper(ncent, centbins);
	}
	jetpt_min = 40;
	jeteta_max = 2.0;
	btagger->recoJetCut = recoJetCuts;
	btagger->probeCSV("csv0p9", 0.9);
	btagger->jtWeight = jtptWf;
	ts->addPlugin(btagger);
	ts->evtWeight = evtWeight0;
	//ts->evtWeight = inclJetEvtWeight;
	ts->evtCut = evtCut1;
	ts->nevt= -1;
	ts->run();
}
