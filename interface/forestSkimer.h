#ifndef FORESTSKIMER_h
#define FORESTSKIMER_h

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "TTree.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class forestSkimer : public edm::EDAnalyzer {
	public:
		explicit forestSkimer(const edm::ParameterSet& p);
		~forestSkimer();

	private:
		virtual void beginJob() override;
		virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
		virtual void endJob() override;
		TTree *itree;
		TTree *otree;
		TFile *of;
		TFileService *sf;

		float ijtpt[999];

		std::vector<float> *jtpt;
};

#endif
