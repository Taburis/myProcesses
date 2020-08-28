
#include "myProcesses/hiforest/plugin/eventMap_skim.h" // for the skim file
//#include "myProcesses/hiforest/plugin/eventMap_hiForest.h" // for the hiforest file
#include "myProcesses/jtc/config/config_AN20_029.h"
#include "myProcesses/jtc/plugin/QAScanner.h"


void qacfg(TString infname="", TString output="output.root"){
	if(infname =="")infname = "root://xrootd.rcac.purdue.edu//store/user/wangx/PH2018_JetSamples/Bjet_pThat-15_TuneCP5_HydjetDrumMB_5p02TeV_Pythia8/bjetSkim_purdue_run1/200325_155632/0000/skim_105.root" ;
	auto f = TFile::Open(infname);
	auto em = new eventMap(f);
	em->isMC = 1;
        em->AASetup = 1;
        em->init();
	em->loadJet("ak4PFJetAnalyzer");
	cout<<em->evtTree->GetEntriesFast()<<endl;

	auto ts = new treeScanner(em);
	auto qa = new QAScanner("jetQA");
	float hibin[] ={0,60,180};

	ts->evtCut = config::evtCut;
	ts->outputName = output;
	ts->evtWeight = config::pthatWeight;
	qa->addHiBins(2,hibin);
	ts->addPlugin(qa);
	ts->run();
}
