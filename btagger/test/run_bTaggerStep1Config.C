
#include "../plugin/treeScanner.h"
#include "../plugin/bTaggerAnalyzer.h"
#include "../plugin/config_aa2018_bjet.h"

void run_bTaggerStep1Config(TString inf="/eos/cms/store/group/phys_heavyions/ikucher/bjetFrac/forBtag/mc_dijet/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_FlavorFix_dir0001_part6_9.root", TString outf="AA2018bTagger_id1"){
	auto f = TFile::Open(inf);

	using namespace AA2018bJet;
	config_init();
	Double_t csvWPs[4] = {.375, .8, .9625, 1};

	eventMap *em = new eventMap(f);
	em->isMC = 0;
	em->AASetup = 1;
	em->init();
	em->regEventFilter(nfilter, filters);
	treeScanner *ts = new treeScanner(em);

	auto btagger  = new bTaggerAnalyzer(outf);
	btagger->scheduleJetSet("akFlowPuCs4PFJetAnalyzer");
	btagger->cent = new centralityHelper(ncent, centbins);
	btagger->recoJetCut = recoJetCut;
cout<<"here"<<endl;
	btagger->addProbeWPs(3, csvWPs);
cout<<"here"<<endl;
	ts->addPlugin(btagger);
	ts->evtWeight = inclJetEvtWeight;
	ts->evtCut = basicEvtCuts;
	ts->nevt=-1;
	ts->run();
}
