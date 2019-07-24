#ifndef FORESTSKIMER_H
#define FORESTSKIMER_H

// system include files
#include <memory>
#include <string>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


class forestSkimer : public edm::EDAnalyzer {
	public:
		explicit forestSkimer(const edm::ParameterSet&);
		~forestSkimer();

		virtual void analyze(const edm::Event&, const edm::EventSetup&);
		virtual void endJob();

	private:
		TTree *itree;
		TTree *otree;
		TFile *of;//, *infile;
		edm::Service<TFileService> sf;
		float ijtpt[999];
		std::vector<float> *jtpt;

};

#endif
