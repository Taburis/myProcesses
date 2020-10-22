#include "myProcesses/hiforest/interface/forestSkimer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
//#include <Math/DistFunc.h>
#include "TMath.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

using namespace std;
using namespace edm;


forestSkimer::forestSkimer(const edm::ParameterSet& iConfig) :
	filters (iConfig.getParameter<std::vector<std::string>> ("event_filter")),
	doBtag (iConfig.getParameter<bool>("doBtag"))
{
	_jetname = iConfig.getParameter<std::string>("jetset");
	doTrk = iConfig.getParameter<bool>("doTrk");
	ispp = iConfig.getParameter<bool>("isPP");
	isMC = iConfig.getParameter<bool>("isMC");

	std::vector<int> muonInfo= iConfig.getParameter<std::vector<int>>("muonInfo");
	addMuon      = muonInfo[0];
	fullMuonInfo = muonInfo[1];

	const ParameterSet trkPSet = iConfig.getParameter<edm::ParameterSet>("trkCuts");
	const ParameterSet recoJetPSet = iConfig.getParameter<edm::ParameterSet>("recoJetCuts");
	const ParameterSet GPPSet = iConfig.getParameter<edm::ParameterSet>("GPCuts");

	//trk cut
	trkptmin = trkPSet.getParameter    <double>("trkptmin");
	trkptmax = trkPSet.getParameter    <double>("trkptmax");
	trketamax = trkPSet.getParameter   <double>("trketamax");
	trkptsig = trkPSet.getParameter    <double>("trkptsig");
	trknhitmin = trkPSet.getParameter  <int>("trknhitsmin");
	trkchi2max = trkPSet.getParameter  <double>("trkchi2max");
	doHighpurity = trkPSet.getParameter<bool>("doHighPurity");
	doCaloMatch = trkPSet.getParameter <bool>("doCaloMatch");

	//jet cut 	
	jetptmin = recoJetPSet.getParameter <double>("jetptmin");
	jetetamax = recoJetPSet.getParameter<double>("jetetamax");
	//gen particle cut 	
	genptmin = GPPSet.getParameter<double>("gpPtMin");
	genetamax = GPPSet.getParameter<double>("gpEtaMax");
	keepNeutral = GPPSet.getParameter<bool>("keepNeutral");

	std::cout<<"==========================================================doBtag: "<<doBtag<<std::endl;
}


void forestSkimer::analyze(const Event& iEvent,	 const EventSetup& iSetup) {}

forestSkimer::~forestSkimer() {
	cout<<"deleting module: forestSkimer"<<endl;
	delete em;
	//	if(of) of->Close();
	//	otree->ResetBranchAddresses(); delete otree;
	cout<<"deleting module: forestSkimer, finished"<<endl;
}

bool forestSkimer::recoJetCut(eventMap *em, int j){
	if( em->jetpt[j] < jetptmin) return 1;
	if(fabs(em->jeteta[j]) > jetetamax) return 1;
	return 0;
}
bool forestSkimer::genJetCut(eventMap *em, int j){
	if( em->genjetpt[j] < jetptmin) return 1;
	if(fabs(em->genjeteta[j]) > jetetamax) return 1;
	return 0;
}

bool forestSkimer::genParticleCut(eventMap *em, int j){
	if(em->gppt(j) < genptmin) return 1;
	if(fabs(em->gpeta(j)) > genetamax) return 1;
	if(em->gpchg(j) ==0 && !keepNeutral) return 1;
	return 0;
}

bool forestSkimer::trkCut(eventMap *em, int j){
	if(em->trkpt[j] < trkptmin || em->trkpt[j]>trkptmax) return 1;
	if(TMath::Abs(em->trketa[j]) >= trketamax) return 1;
	if((!(em->highPurity[j])) && doHighpurity) return 1;
	if(em->trknhit[j]< trknhitmin) return 1;
	if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > trkchi2max) return 1;
	float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
	if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j])) && doCaloMatch) return 1;
	return 0;

}

void forestSkimer::initEventMap(){
	em->isHI = !ispp;
	em->isMC = isMC;
	em->init();
	em->loadTrack();
	if(isMC) em->loadGenParticle();
	em->loadJet(_jetname.c_str());
	if(addMuon) em->loadMuons(fullMuonInfo);
	em->loadBTagger();
	em->regEventFilter(filters);
}

void forestSkimer::fillBtagInfo() {
	GSPevt = em->GSPevt;
	ngenV0 = em->ngenV0;
	for(int i=0; i<em->ngenV0; ++i){
		genV0_pt [i]= em->genV0_pt[i];
		genV0_eta[i]= em->genV0_eta[i];
		genV0_phi[i]= em->genV0_phi[i];
		genV0_pdgID [i]= em->genV0_pdgID[i];
		genV0_ncharged[i]= em->genV0_ncharged[i];
		genV0_SVx[i] = em->genV0_SVx[i];
		genV0_SVy[i] = em->genV0_SVy[i];
		genV0_SVz[i] = em->genV0_SVz[i];
	}
}

void forestSkimer::endJob() {

	TFile *infile = &(sf->file());

	em = new eventMap(infile);
	initEventMap();

	cout<<" opening input forst file "<<infile->GetName()<<endl;

	//take this to config file ultimately


	of = TFile::Open("skim.root", "recreate");
	otree = new TTree("mixing_tree", "");
	buildOuttree();
	loadJets(jet0);
	if(addMuon) addMuonBranch(fullMuonInfo);
	long nevt = em->evtTree->GetEntriesFast();
	for(int ievt = 0; ievt < nevt; ievt++){
		std::cout<<"Processing event "<<ievt<<"...."<<std::endl;
		em->getEvent(ievt);
		/*
		*/
		if(em->checkEventFilter()) continue;
		if(onlyJetEvent){
			int foundGenJet = 0, foundRecoJet = 0;
			for(int j1 = 0; j1 < em->nJet() ; j1++)
			{
				if(recoJetCut(em,j1)) continue;
				foundRecoJet = 1; break;
			}
			if(isMC){
				for(int j=0; j< em->nGenJet(); j++){
					if(genJetCut(em,j)) continue;
					foundGenJet = 1; break;
				}
			}
			if( !foundRecoJet && !isMC) continue;
			if( !foundRecoJet && (isMC && !foundGenJet)) continue;
		}

		int counter = 0;
		for(int j1 = 0; j1 < em->nJet() ; j1++)
		{
			if(recoJetCut(em,j1)) continue;

			jet0.jeteta[counter]=em->jeteta[j1];
			jet0.jetphi[counter]=em->jetphi[j1];
			jet0.jetpt [counter]=em->jetpt [j1];
			jet0.jet_wta_eta [counter]=em->jet_wta_eta [j1];
			jet0.jet_wta_phi [counter]=em->jet_wta_phi [j1];

			jet0.trackMax    [counter]=em->trkPtMax[j1];
			jet0.discr_csvV2 [counter]=em->disc_csvV2[j1];
			jet0.pdiscr_csvV2[counter]=em->pdisc_csvV2[j1];
			jet0.ndiscr_csvV2[counter]=em->ndisc_csvV2[j1];
			jet0.matchedHadronFlavor[counter]=em->matchedHadronFlavor[j1];
			jet0.matchedPartonFlavor[counter]=em->matchedPartonFlavor[j1];
			jet0.genMatchIndex[counter]=em->genMatchIndex[j1];
			jet0.bHadronNumber[counter]=em->bHadronNumber[j1];
			if(isMC){
				jet0.refpt[counter]=em->ref_jetpt[j1];
			}
			counter++;
		}
		jet0.njet = counter;

		int ijet=0;
		if(isMC){
			if(doBtag) fillBtagInfo();
			for(int j=0; j< em->nGenJet(); j++){
				if(genJetCut(em,j)) continue;
				jet0.genjetpt      [ijet]=em->genjetpt      [j];
				jet0.genjetphi     [ijet]=em->genjetphi     [j];
				jet0.genjeteta     [ijet]=em->genjeteta     [j];
				jet0.genjet_wta_eta[ijet]=em->genjet_wta_eta[j];
				jet0.genjet_wta_phi[ijet]=em->genjet_wta_phi[j];
				ijet++;
			}
			jet0.ngj = ijet;
			for(int j=0; j< em->nGP(); j++){
				if(genParticleCut(em, j)) continue;
				gpptp.emplace_back(em->gppt(j));
				gpetap.emplace_back(em->gpeta(j));
				gpphip.emplace_back(em->gpphi(j));
				gpchgp.emplace_back(em->gpchg(j));
				gppdgIDp.emplace_back(em->gppdgID(j));
				if(!stableOnly)gpStableTag.emplace_back(em->gpIsStable(j));
				if(!ispp) gpsube.emplace_back(em->gpSube(j));
			}
		}

		int itrk = 0;
		 ntrk=0;
		for(int i=0; i<em->nTrk(); ++i){
			if(trkCut(em, i)) continue;
			trkpt [itrk]=em->trkpt[i];
			trkphi[itrk]=em->trkphi[i];
			trketa[itrk]=em->trketa[i];
			trkpterr  [itrk]=em->trkpterr[i];
			trkchi2   [itrk]=em->trkchi2[i];
			trknhit   [itrk]=em->trknhit[i];
			trknlayer [itrk]=em->trknlayer[i];
			trkndof   [itrk]=em->trkndof[i];
			highPurity[itrk]=em->highPurity[i];
			pfEcal    [itrk]=em->pfEcal[i];
			pfHcal    [itrk]=em->pfHcal[i];
			itrk++;
		}
		ntrk = itrk;

		int imuon = 0;
		nMuon=0;
		if(addMuon){
			for(int i=0; i<em->nMuon; i++){

				muonPt            [imuon] = em->muonPt        ->at(i);
				muonEta           [imuon] = em->muonEta       ->at(i);
				muonPhi           [imuon] = em->muonPhi       ->at(i);
				muonCharge        [imuon] = em->muonCharge    ->at(i);
				muonType          [imuon] = em->muonType      ->at(i);
				muonIsGood        [imuon] = em->muonIsGood    ->at(i);

				muonIsGlobal      [imuon] = em->muonIsGlobal  ->at(i);
				muonIsTracker     [imuon] = em->muonIsTracker ->at(i);
				muonIsPF          [imuon] = em->muonIsPF      ->at(i);
				muonIsSTA         [imuon] = em->muonIsSTA     ->at(i);

				muonD0            [imuon] = em->muonD0        ->at(i);
				muonDz            [imuon] = em->muonDz        ->at(i);
				muonD0Err         [imuon] = em->muonD0Err     ->at(i);
				muonDzErr         [imuon] = em->muonDzErr     ->at(i);
				muonIP3D          [imuon] = em->muonIP3D      ->at(i);
				muonIP3DErr       [imuon] = em->muonIP3DErr   ->at(i);
				muonChi2NDF       [imuon] = em->muonChi2NDF   ->at(i);

				if(fullMuonInfo){
					muonInnerD0       [imuon] = em->muonInnerD0   ->at(i);
					muonInnerDz       [imuon] = em->muonInnerDz   ->at(i);
					muonInnerD0Err    [imuon] = em->muonInnerD0Err->at(i);
					muonInnerDzErr    [imuon] = em->muonInnerDzErr->at(i);
					muonInnerPt       [imuon] = em->muonInnerPt   ->at(i);
					muonInnerPtErr    [imuon] = em->muonInnerPtErr->at(i);
					muonInnerEta      [imuon] = em->muonInnerEta  ->at(i);

					muonTrkLayers     [imuon] = em->muonTrkLayers  ->at(i);
					muonPixelLayers   [imuon] = em->muonPixelLayers->at(i);
					muonPixelHits     [imuon] = em->muonPixelHits  ->at(i);
					muonMuonHits      [imuon] = em->muonMuonHits   ->at(i);
					muonTrkQuality    [imuon] = em->muonTrkQuality ->at(i);
					muonStations      [imuon] = em->muonStations   ->at(i);
					muonIsoTrk        [imuon] = em->muonIsoTrk     ->at(i);
					muonPFChIso       [imuon] = em->muonPFChIso    ->at(i);
					muonPFPhoIso      [imuon] = em->muonPFPhoIso   ->at(i);
					muonPFNeuIso      [imuon] = em->muonPFNeuIso   ->at(i);
					muonPFPUIso       [imuon] = em->muonPFPUIso    ->at(i);

					muonIDSoft        [imuon] = em->muonIDSoft     ->at(i);
					muonIDLoose       [imuon] = em->muonIDLoose    ->at(i);
					muonIDMedium      [imuon] = em->muonIDMedium   ->at(i);
					muonIDMediumPrompt[imuon] = em->muonIDMediumPrompt->at(i);
					muonIDTight       [imuon] = em->muonIDTight       ->at(i);
					muonIDGlobalHighPt[imuon] = em->muonIDGlobalHighPt->at(i);
					muonIDTrkHighPt   [imuon] = em->muonIDTrkHighPt   ->at(i);
					muonIDInTime      [imuon] = em->muonIDInTime      ->at(i);
				}
				imuon++;
			}
			nMuon = imuon;
		}

		std::cout<<"Jet selected, fill this event."<<std::endl;
		if(addMuon)std::cout<<"number of muon: "<<nMuon<<std::endl;
		otree->Fill();
		//clear all the vectors 
		clearTrk();
	}
	of->Write();
	of->Close();
	infile->DeleteAll();
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(forestSkimer);
