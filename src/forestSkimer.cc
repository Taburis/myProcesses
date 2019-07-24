
#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TMath.h"
#include "TString.h"
#include "TTree.h"



#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Handle.h"

#include "CommonTools/Utils/interface/PtComparator.h"
#include "HeavyIonsAnalysis/myProcesses/interface/forestSkimer.h"

using namespace edm;

forestSkimer::forestSkimer(const edm::ParameterSet& iConfig) 
{
	//itree =(TTree*) infile->Get("akPu4PFJetAnalyzer/t");
	//itree->SetBranchAddress("jtpt", &ijtpt);
	//auto infile = sf->file();
	//of = TFile::Open("skim.root", "recreate");
	//otree = new TTree("mixing_tree", "");
	//otree->Branch("jtpt", &jtpt);
}

void forestSkimer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
	std::cout<<"XXXX=============================="<<std::endl;
}

void 
forestSkimer::endJob() {
	std::cout<<"ENDXX============================="<<std::endl;
}

DEFINE_FWK_MODULE(forestSkimer);
