
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
//#include "myProcesses/hiforest/plugin/eventMap_skim.h"
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

void cfg_randomCone(TString inf="root://eoscms.cern.ch//store/group/phys_heavyions/jviinika/PbPb2018MC_Dijet_pThat15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_HINPbPbAutumn18-mva98_103X/0009/HiForestAOD_PbPbMC2018_4985.root", TString outf="randomConeScan.root"){
	fcentNew->SetParameters(7.85523,-.289198,.00449837,-3.77752e-05,1.80269e-07,-4.62581e-10,4.97671e-13);	
	auto f = TFile::Open(inf);
	eventMap *em = new eventMap(f);
	em->isMC = 1;
	em->init();
	em->AASetup = 1;
	em->loadJet("akFlowPuCs4PFJetAnalyzer");
	em->loadTrack();
	em->regEventFilter(nEvtFilter, evtFilters);
	treeScanner *ts = new treeScanner(em);
	ts->reportPercent = 0;
	auto cent = new centralityHelper(ncent,centbins);
	auto jtR = new jetRecoAnalyzer("randomConeAna");
	jtR->output_name = outf;
	jtR->cent = cent;
	jtR->trkCuts = trkCuts;
	jtR->recoJtCuts = recoJetCuts;

	ts->evtCut = evtCut1;
	ts->evtWeight = evtWeight0;
	ts->addPlugin(jtR);
	ts->nevt= -1;
	ts->run();
}
