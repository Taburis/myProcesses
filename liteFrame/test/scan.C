
#define event_content_skim
#include "cfg.h"
#include "myProcesses/jtc/plugin/producerBJTC.h"

void scan(TString input = "", TString output="test.root"){

        if(input == "") input = "root://eoscms.cern.ch//store/group/phys_heavyions/wangx/PH2018_bJetSample/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_purdue_run2/200915_211050/0000/skim_101.root";

	auto f = TFile::Open(input);
	config_AN20029::PSet_AN20029_v0 pset;
	config_AN20029::selections src;
	auto lf = new liteFrame<eventMap>("test", pset, src, f);
	auto jp = new producerBJTC<eventMap>();
	lf->addProducer(jp);
}
