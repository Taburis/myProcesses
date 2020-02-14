
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#include "myProcesses/jtc/plugin/treeScanner.h"
#include "myProcesses/jtc/config/config_aa2018_bjet.h"
#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/jetRecoAnalyzer.h"
using namespace AA2018bJet;
TF1* fcentNew = new TF1("f","pol6", 0,200);
TH1D* hcsv_weight[4];
TH1D* hinclJtPtW[4];
float evtWeight0(eventMap* em){
	return fvzw.Eval(em->vz)*fcentNew->Eval(em->hiBin);
	return 1;
}

bool evtCut1(eventMap *em){
	if(basicEvtCuts(em)) return 1;
	if(em->isMC) if(em->pthat < 60) return 1;
	return 0;
}

void cfg_randomCone(TString inf="/mnt/hadoop/store/user/wangx/PH2018_JetSamples/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_JetClean_Purdue/200202_224121/0000/skim_525.root", TString outf=".root"){
	fcentNew->SetParameters(7.85523,-.289198,.00449837,-3.77752e-05,1.80269e-07,-4.62581e-10,4.97671e-13);	
	auto f = TFile::Open(inf);
	eventMap *em = new eventMap(f);
	em->isMC = 1;
	em->init();
	em->AASetup = 1;
	treeScanner *ts = new treeScanner(em);
	ts->reportPercent = 0;
	auto cent = new centralityHelper(ncent,centbins);
	auto jtR = new jetRecoAnalyzer("randomConeAna");
	jtR->js_name = "";
	jtR->cent = cent;
	ts->addPlugin(jtR);
	ts->evtWeight = evtWeight0;
	ts->evtCut = evtCut1;
	ts->nevt= -1;
	ts->run();
}
