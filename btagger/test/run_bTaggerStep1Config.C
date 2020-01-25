
#include "myProcesses/jtc/plugin/treeScanner.h"
#include "myProcesses/btagger/plugin/bTaggerAnalyzer.h"
#include "myProcesses/jtc/plugin/config_aa2018_bjet.h"
#include "myProcesses/jtc/plugin/Utility.h"

using namespace AA2018bJet;
TF1* fcentNew = new TF1("f","pol6", 0,200);

float evtWeight(eventMap* em){
	return fvzw.Eval(em->vz)*fcentNew->Eval(em->hiBin);
	//return 1;
	//float weight1 = inclJetEvtWeight(em);
	//return fw2->Eval(em->hiBin)*weight1;
}

void run_bTaggerStep1Config(TString inf="/eos/cms/store/group/phys_heavyions/ikucher/bjetFrac/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_PbPbCSVv2TaggersFixed/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_PbPbCSVv2TaggersFixed/200113_134921/0000/HiForestAOD_PbPbCSVv2TaggersFixed_105.root", TString outf="AA2018bTagger_id1"){
	auto f = TFile::Open(inf);

	bool doshift = 0;
	fcentNew->SetParameters(7.85523,-.289198,.00449837,-3.77752e-05,1.80269e-07,-4.62581e-10,4.97671e-13);	

	float centbins_5shift[] = {10, 30, 70, 110, 200};
	config_init();
	Double_t csvWPs[4] = {.375, .8, .9625, 1};

	eventMap *em = new eventMap(f);
	em->isMC = 1;
	em->AASetup = 1;
	em->init();
	em->regEventFilter(nfilter, filters);
	treeScanner *ts = new treeScanner(em);
	ts->reportPercent = 0;

	auto btagger  = new bTaggerAnalyzer(outf);
	btagger->scheduleJetSet("akFlowPuCs4PFJetAnalyzer");
	if( doshift) {
		btagger->cent = new centralityHelper(ncent, centbins_5shift);
	}
	else {
		btagger->cent = new centralityHelper(ncent, centbins);
	}
	btagger->recoJetCut = recoJetCut;
	btagger->addProbeWPs(3, csvWPs);
	ts->addPlugin(btagger);
	ts->evtWeight = evtWeight;
	//ts->evtWeight = inclJetEvtWeight;
	ts->evtCut = basicEvtCuts;
	ts->nevt=-1;
	ts->run();
}
