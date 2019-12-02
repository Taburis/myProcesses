
#include "../plugin/treeScanner.h"
#include "../plugin/bTaggerAnalyzer.h"
#include "../plugin/config_aa2018_bjet.h"

void run_bTagger_step1_scan(TString inf="/eos/cms/store/group/phys_heavyions/ikucher/bjetFrac/forBtag/mc_dijet/DiJet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8_FlavorFix_dir0000_part1_5.root", TString outf="AA2018bTagger_id1"){
	auto f = TFile::Open(inf);

	using namespace AA2018bJet;

	eventMap *em = new eventMap(f);
	em->isMC = 0;
	em->init();
	if( !em->isMC ){	
		em->regEventFilter(nfilter, filters);
	}
	treeScanner *ts = new treeScanner(em);

	auto btagger  = new bTaggerAnalyzer(outf);
	btagger->scheduleJetSet("akFlowPuCs4PFJetAnalyzer");
	btagger->cent = new centralityHelper(ncent, centbins);
	btagger->recoJetCut = recoJetCut;

	ts->addPlugin(btagger);
	ts->evtCut = basicEvtCuts;
	ts->nevt=-1;
	ts->run();
}
