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
	filters (iConfig.getParameter<std::vector<std::string>> ("event_filter"))
{
	_jetname = iConfig.getParameter<std::string>("jetset");
	doTrk = iConfig.getParameter<bool>("doTrk");
	ispp = iConfig.getParameter<bool>("isPP");
	isMC = iConfig.getParameter<bool>("isMC");

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
	if( em->jetpt[j] < ptmin) return 1;
	if(fabs(em->jeteta[j]) > etamax) return 1;
	return 0;
}
bool forestSkimer::genJetCut(eventMap *em, int j){
	if( em->genjetpt[j] < ptmin) return 1;
	if(fabs(em->genjeteta[j]) > etamax) return 1;
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
	if(!(em->highPurity[j]) && doHighpurity) return 1;
	if(em->trknhit[j]< trknhitmin) return 1;
	if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > trkchi2max) return 1;
	float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
	if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j])) && doCaloMatch) return 1;
	return 0;

}

void forestSkimer::initEventMap(){
	em->init();
		em->loadTrack();
	if(isMC) em->loadGenParticle();
		em->loadJet(_jetname.c_str());
	//	em->regEventFilter(filters);
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
	long nevt = em->evtTree->GetEntriesFast();
	for(int ievt = 0; ievt < nevt; ievt++){
		if(ievt% 100) std::cout<<"Processing event "<<ievt<<"...."<<std::endl;
		em->getEvent(ievt);
		/*
		if(em->checkEventFilter()) continue;

		for(int j1 = 0; j1 < em->nJet() ; j1++)
		{
			if(recoJetCut(em,j1)) continue;

			jet0.jeteta.emplace_back(em->jeteta[j1]);
			jet0.jetphi.emplace_back(em->jetphi[j1]);
			jet0.jetpt .emplace_back(em->jetpt [j1]);

			jet0.trackMax.emplace_back(em->jetTrkMax[j1]);
			jet0.discr_csvV2.emplace_back(em->disc_csvV2[j1]);
			if(isMC){
				jet0.refpt.emplace_back(em->ref_jetpt[j1]);
			}
		}
		for(int i=0; i<em->nTrk(); ++i){
			if(trkCut(em, i)) continue;
			trkpt.emplace_back(em->trkpt[i]);
			trkphi.emplace_back(em->trkphi[i]);
			trketa.emplace_back(em->trketa[i]);
			trkpterr.emplace_back(em->trkpterr[i]);
			trkchi2.emplace_back(em->trkchi2[i]);
			trknhit.emplace_back(em->trknhit[i]);
			trknlayer.emplace_back(em->trknlayer[i]);
			trkndof.emplace_back(em->trkndof[i]);
			highPurity.emplace_back(em->highPurity[i]);
		}
		if(!isMC) continue;
		for(int j=0; j< em->nGenJet(); j++){
			if(genJetCut(em,j)) continue;
			jet0.genjeteta.emplace_back(em->genjetpt[j]);
			jet0.genjetphi.emplace_back(em->genjetphi[j]);
			jet0.genjeteta.emplace_back(em->genjeteta[j]);
			jet0.genjet_wta_eta.emplace_back(em->genjet_wta_eta[j]);
			jet0.genjet_wta_phi.emplace_back(em->genjet_wta_phi[j]);
		}
		for(int j=0; j< em->nGP(); j++){
			if(genParticleCut(em, j)) continue;
			gpptp.emplace_back(em->gppt(j));
			gpetap.emplace_back(em->gppt(j));
			gpphip.emplace_back(em->gppt(j));
			gpchgp.emplace_back(em->gpchg(j));
			gppdgIDp.emplace_back(em->gppdgID(j));
		}
		*/
	//	otree->Fill();
		//clear all the vectors 
		clearJetset(jet0);
		clearTrk();
		//infile->Close();
	}
	//	of->Write();
	//otree->Write();
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(forestSkimer);
