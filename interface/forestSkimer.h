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
		void buildIntree();
		bool check_filter();
		void buildOuttree();

	private:
		TTree *itree;
		TTree *mutree;
		TTree *eventtree;

		TTree *otree;
		TFile *of;//, *infile;
		edm::Service<TFileService> sf;

		//0. Event Variables (Variable names are same for input and output trees)
		UInt_t          run;
		ULong64_t       evt;
		UInt_t          lumi;

		Float_t vx, vy, vz;
		Int_t hiBin;

		// for event filters
		TTree* filterTree;
		std::vector<std::string> filterName;
		std::vector<Int_t> filters;

		bool isMC = 0;


		bool doJets =0;
		bool ispp = 0;
		//pf jet info (input tree)
		static const int MAXJETS = 1000;
		float ijtpt[MAXJETS];

		Int_t pf_nref;
		Float_t t_pf_jtpt[MAXJETS], t_pf_jteta[MAXJETS], t_pf_jtphi[MAXJETS], t_pf_trackMax[MAXJETS];
		Float_t t_pf_discr_csvV1[MAXJETS], t_pf_discr_csvV2[MAXJETS];
		Float_t t_pf_wta_eta[MAXJETS], t_pf_wta_phi[MAXJETS];


		//pf jet info (output tree)
		std::vector<float> *jtpt;
		std::vector<float> pf_jteta, pf_jtphi, pf_jtpt, pf_trackMax;
		std::vector<float> pf_discr_csvV1, pf_discr_csvV2;
		std::vector<float> pf_wta_jteta, pf_wta_jtphi;


		// track part
		bool doTrk = 0;
		std::vector<float> trkpt, trketa, trkphi, trkchi2, trkpterr;
		std::vector<int> highPurity, trkndof, trknlayer, trknhit;

		//MC truth
		std::vector<float> genpt, geneta, genphi; 
		std::vector<int>genchg;
		std::vector<float> genjtpt, genjteta, genjtphi, wtageneta, wtagenphi;

		//muon info.
		bool doMuon = 0;
		std::vector<float> *muonPt=0, *muonEta=0, *muonPhi=0; 
		std::vector<int> *muonIsGood=0, *muonType=0, *muonIsGlobal=0, *muonIsTracker=0, *muonIsPF=0, *muonIsSTA=0, *muonCharge=0;
		std::vector<float> *muonD0=0, *muonDz=0, *muonIP3D=0, *muonD0Err=0, *muonDzErr=0, *muonIP3DErr=0, *muonChi2ndf=0;
		std::vector<float> *muonInnerDz=0, *muonInnerD0=0, *muonInnerDzErr=0, *muonInnerD0Err=0, *muonInnerPt=0, *muonInnerPtErr=0;
		std::vector<float> *muonInnerEta=0;
		std::vector<int> *muonTrkLayers=0, *muonPixelLayers=0, *muonPixelHits=0, *muonMuHits=0, *muonTrkQuality=0, *muonStations=0;






};
#endif
