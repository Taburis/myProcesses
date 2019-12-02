
#include "bTaggerAnalyzer.h"
#include "config_aa2018_bjet.h"

void test_step2(){
	auto f = TFile::Open("AA2018bTagger_step1.root");
	using namespace AA2018bJet;
	
	auto btagger  = new bTaggerAnalyzer("AA2018bTagger");
	btagger->cent = new centralityHelper(ncent, centbins);
	btagger->loadStep1File(f);
	btagger->drawStack("c_jetpt", btagger->jtpt);
//	btagger->drawStack("c_jetpt", btagger->disc);
}
