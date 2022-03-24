#include "myProcesses/hiTracking/interface/trackingMVA_skimer.h"
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/TriggerNamesService.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/OneToOne.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
//#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/EncodedEventId/interface/EncodedEventId.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderNoMaterial.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include <TrackingTools/PatternTools/interface/TSCPBuilderNoMaterial.h>
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "DataFormats/TrackReco/interface/TrackResiduals.h"
#include "TMath.h"

#include <functional>
#include <sstream>
#include <iostream>

typedef math::XYZTLorentzVectorD LorentzVector;
typedef math::XYZVector Vector;
typedef math::XYZPoint Point;

using namespace edm;
using namespace reco;
using namespace std;

trackingMVA_skimer::trackingMVA_skimer(const edm::ParameterSet& Params)
{
		outFile = 0;
		source = "generalTracks";
		associatorName = "TrackAssociatorByHits";
		mvaValues = &d_mvaValues;
		beamspot_ = Params.getParameter<edm::InputTag>("beamspot");

		if(Params.exists("outfile")) outFile = new TFile(Params.getParameter<string>("outfile").c_str(),"RECREATE");
		if(Params.exists("source")) source = Params.getParameter<string>("source");
		if(Params.exists("associator")) associatorName = Params.getParameter<string>("associator");
		if(Params.exists("simSource")) simSource = Params.getParameter<InputTag>("simSource");
		if(Params.exists("vertices")) vertices = consumes<reco::VertexCollection>(Params.getParameter<edm::InputTag>("vertices"));

		beamspotToken = consumes<reco::BeamSpot>(beamspot_);
		associatorToken = consumes<reco::TrackToTrackingParticleAssociator>(edm::InputTag(associatorName));
		trackToken = consumes<edm::View<Track> > (edm::InputTag(source));
		trackHighPurityToken = consumes<edm::View<Track> >(edm::InputTag("selectHighPurity"));
		simTPToken = consumes<TrackingParticleCollection>(simSource);

		doMVA_ = false;
		//	std::string mvaFileName;
		vector<string> mvaFileNames;
		if(Params.exists("doMVA")) doMVA_ = Params.getParameter<bool>("doMVA");
		if(Params.exists("mvaType")) mvaTypes_ = Params.getParameter<vector<string> >("mvaType");
		if(Params.exists("mvaFileName")) mvaFileNames = Params.getParameter<vector<string> >("mvaFileName");

		if(doMVA_){
				for(unsigned int i = 0; i < mvaTypes_.size(); i++){
						TMVA::Reader* tmvaReader_ = new TMVA::Reader("!Color:Silent");
						tmvaReader_->AddVariable("pt",&tvpt);
						tmvaReader_->AddVariable("lostmidfrac",&tvLostMidFrac);
						tmvaReader_->AddVariable("minlost",&tvMinLost);
						tmvaReader_->AddVariable("nhits",&tvNhits);
						tmvaReader_->AddVariable("relpterr",&tvRelPtErr);
						tmvaReader_->AddVariable("eta",&tvEta);
						//tmvaReader_->AddVariable("phi",&tvPhi);
						tmvaReader_->AddVariable("chi2n_no1Dmod",&tvChi2n_no1Dmod);
						tmvaReader_->AddVariable("chi2n",&tvChi2n);
						tmvaReader_->AddVariable("nlayerslost",&tvNlayersLost);
						tmvaReader_->AddVariable("nlayers3D",&tvNlayers3D);
						tmvaReader_->AddVariable("nlayers",&tvNlayers);
						tmvaReader_->AddVariable("ndof",&tvNdof);
						if(mvaTypes_[i] == "Prompt"){
								tmvaReader_->AddVariable("absd0PV",&tvAbsD0PV);
								tmvaReader_->AddVariable("absdzPV",&tvAbsDzPV);
								tmvaReader_->AddVariable("absdz",&tvAbsDz);
								tmvaReader_->AddVariable("absd0",&tvAbsD0);
						}

						tmvaReader_->BookMVA("BDTG",mvaFileNames[i]);
						tmvaReaders_.push_back(tmvaReader_);
				}
		}
		if(makeSimTree_){
				simTree = new TTree("simTree","",1);
				simTree->Branch("passTrkCut" ,&passTrkCut ,"passTrkCut/O");
				simTree->Branch("sim_rec" ,&sim_rec ,"sim_rec/I");
				simTree->Branch("sim_pt" ,&sim_pt ,"sim_pt/F");
				simTree->Branch("sim_eta",&sim_eta,"sim_eta/F");
				simTree->Branch("sim_phi",&sim_phi,"sim_phi/F");
		}

		if(makeMVATree_){
				outTree = new TTree("NtupleTree","",1);
				outTree->Branch("fake",&tvFake,"fake/F");
				outTree->Branch("iter",&tvIter,"iter/F");
				outTree->Branch("ndof",&tvNdof,"ndof/F");
				outTree->Branch("pt",&tvpt,"pt/F");
				outTree->Branch("nlayers",&tvNlayers,"nlayers/F");
				outTree->Branch("nlayers3D",&tvNlayers3D,"nlayers3D/F");
				outTree->Branch("nlayerslost",&tvNlayersLost,"nlayerslost/F");
				outTree->Branch("chi2n",&tvChi2n,"chi2n/F");
				outTree->Branch("chi2n_no1Dmod",&tvChi2n_no1Dmod,"chi2n_no1Dmod/F");
				outTree->Branch("eta",&tvEta,"eta/F");
				//outTree->Branch("phi",&tvPhi,"phi/F");
				outTree->Branch("relpterr",&tvRelPtErr,"relpterr/F");
				outTree->Branch("nhits",&tvNhits,"nhits/F");
				outTree->Branch("lostin",&tvLostIn,"lostin/F");
				outTree->Branch("lostout",&tvLostOut,"lostout/F");
				outTree->Branch("minlost",&tvMinLost,"minlost/F");
				outTree->Branch("lostmidfrac",&tvLostMidFrac,"lostmidfrac/F");
				outTree->Branch("absdz",&tvAbsDz,"absdz/F");
				outTree->Branch("absd0",&tvAbsD0,"absd0/F");
				outTree->Branch("absdzPV",&tvAbsDzPV,"absdzPV/F");
				outTree->Branch("absd0PV",&tvAbsD0PV,"absd0PV/F");
				outTree->Branch("mvavals","std::vector<float>",&mvaValues);
				outTree->Branch("isHP",&isHP, "isHP/I");
		}

		consumes<reco::TrackToTrackingParticleAssociator>(edm::InputTag(associatorName));

}


trackingMVA_skimer::~trackingMVA_skimer()
{

		// do anything here that needs to be done at desctruction time
		// (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
		void
trackingMVA_skimer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
		edm::Handle<reco::BeamSpot> Bsp;
		iEvent.getByToken(beamspotToken,Bsp);
		reco::BeamSpot vertexBeamSpot;
		vertexBeamSpot = *Bsp;

		edm::Handle<reco::TrackToTrackingParticleAssociator> assocHandle;
		iEvent.getByToken(associatorToken,assocHandle);
		associator = assocHandle.product();

		iSetup.get<IdealMagneticFieldRecord>().get(magfield);

		edm::Handle<reco::VertexCollection> Vtx;
		iEvent.getByToken(vertices,Vtx);

		edm::Handle<View<Track> > handle;
		iEvent.getByToken(trackToken,handle);
		edm::Handle<View<Track> > handleHighPurity;
		iEvent.getByToken(trackHighPurityToken,handleHighPurity);
		edm::Handle<TrackingParticleCollection> simTPhandle;
		iEvent.getByToken(simTPToken,simTPhandle);
		const TrackingParticleCollection simTracks = *(simTPhandle.product());

		reco::RecoToSimCollection recSimColl;
		reco::SimToRecoCollection simRecColl;

		recSimColl = associator->associateRecoToSim(handle,simTPhandle);
		simRecColl = associator->associateSimToReco(handle,simTPhandle);

		auto vtxVector  = Vtx.product();
		reco::VertexCollection vsorted = *Vtx;
		std::sort(vsorted.begin(), vsorted.end(), trackingMVA_skimer::sortVertex);

		if(makeSimTree_){
				/*
				   for(auto & p : simTracks){
				   if(p.status() < 0 || p.charge()==0) continue;
				   sim_pt = p.pt();	
				   sim_eta = p.eta();	
				   sim_phi = p.phi();	
				   simTree->Fill();	
				   }
				   */
				bool doCaloMatched_ = true;
				Handle<edm::View<reco::Track> > tcol;
				for(TrackingParticleCollection::size_type j=0 ; j<simTPhandle->size(); j++){
						TrackingParticleRef tpr(simTPhandle, j);
						TrackingParticle* p = const_cast<TrackingParticle*>(tpr.get());
						if(p->status() < 0 || p->charge()==0) continue;
						Int_t nrec=0;
						passTrkCut = 0 ;
						if(simRecColl.find(tpr) != simRecColl.end()){
								auto rt = (std::vector<std::pair<edm::RefToBase<reco::Track>, double> >) simRecColl[tpr];
								std::vector<std::pair<edm::RefToBase<reco::Track>, double> >::const_iterator rtit;
								nrec = rt.size();
								for (rtit = rt.begin(); rtit != rt.end(); ++rtit){
										const reco::Track* tmtr = rtit->first.get();
										if( ! hiTrkCuts(*tmtr, vsorted[0]) ) continue;
										unsigned index = -1;
										//if( doCaloMatched_ ){ 
										//		for(edm::View<reco::Track>::size_type i=0; i<tcol->size(); ++i){ 
										//				edm::RefToBase<reco::Track> track(tcol, i);
										//				reco::Track* tr=const_cast<reco::Track*>(track.get());
										//				index++;
										//				if( tmtr->pt() == tr->pt() && tmtr->eta() == tr->eta() && tmtr->phi() == tr->phi() && tmtr->numberOfValidHits() == tr->numberOfValidHits() ) break;//simple match to find the corresponding index number (i-th track) in the track collection
										//		}
										//		if( ! caloMatched(*tmtr, iEvent, index) ) continue;
										//}  
										passTrkCut = 1;
										break;
										/*
										*/
								}
						}
						sim_rec = nrec;
						sim_pt  = p->pt();	
						sim_eta = p->eta();	
						sim_phi = p->phi();	
						simTree->Fill();
				}
		}

		for (int i = 0; i<(int)handle->size(); i++){
				mvaValues->clear();
				mvaValues->reserve(mvaTypes_.size());
				Track tk = (handle->at(i));
				tvFake = 1;
				isHP = Int_t(tk.quality(reco::TrackBase::qualityByName("highPurity")));
				tvNdof = tk.ndof();
				tvNlayers = tk.hitPattern().trackerLayersWithMeasurement();
				tvNlayers3D = tk.hitPattern().pixelLayersWithMeasurement() + tk.hitPattern().numberOfValidStripLayersWithMonoAndStereo();
				tvNlayersLost = tk.hitPattern().trackerLayersWithoutMeasurement(HitPattern::TRACK_HITS);

				float chi2n = tk.normalizedChi2();
				float chi2n_no1Dmod = chi2n;

				int count1dhits = 0;

				for (trackingRecHit_iterator ith = tk.recHitsBegin(), edh = tk.recHitsEnd(); ith != edh; ++ith)
				{
						const TrackingRecHit * hit = (*ith);
						if (hit->isValid())
						{
								if (typeid(*hit) == typeid(SiStripRecHit1D)) ++count1dhits;
						}
				}
				if (count1dhits > 0)
				{
						float chi2 = tk.chi2();
						float ndof = tk.ndof();
						chi2n = (chi2+count1dhits)/float(ndof+count1dhits);
				}

				tvChi2n = chi2n;
				tvChi2n_no1Dmod = chi2n_no1Dmod;
				tvEta = tk.eta();
				//tvPhi = tk.phi();
				tvpt = tk.pt();
				tvRelPtErr = float(tk.ptError())/std::max(float(tk.pt()),0.000001f);
				tvNhits = tk.numberOfValidHits();
				tvLostIn = tk.hitPattern().numberOfLostTrackerHits(reco::HitPattern::MISSING_INNER_HITS);
				tvLostOut = tk.hitPattern().numberOfLostTrackerHits(reco::HitPattern::MISSING_OUTER_HITS);
				tvMinLost = std::min(tvLostIn, tvLostOut);
				tvLostMidFrac = float(tk.numberOfLostHits()) / float(tk.numberOfValidHits() + tk.numberOfLostHits());

				RefToBase<Track> trackRef1(handle,i);

				tvAbsDz = fabs(tk.dz(vertexBeamSpot.position()));
				tvAbsD0 = fabs(tk.dxy(vertexBeamSpot.position()));

				int vdx = getBestVertex(trackRef1,*(vtxVector));
				Point VertexPosition(0,0, -99999); 
				if(vdx >-1) VertexPosition = vtxVector->at(vdx);
				tvAbsDzPV = fabs(tk.dz (VertexPosition));
				tvAbsD0PV = fabs(tk.dxy(VertexPosition));

				TString algoName(tk.algoName());
				if (algoName == "initialStep"){
						tvIter = 0;
				}else if(algoName == "highPtTripletStep"){
						tvIter = 1;
				}else if(algoName == "lowPtQuadStep"){
						tvIter = 2;
				}else if(algoName == "lowPtTripletStep"){
						tvIter = 3;
				}else if(algoName == "pixelPairStep"){
						tvIter = 4;
				}else if(algoName == "detachedTripletStep"){
						tvIter = 5;
				}else if(algoName == "mixedTripletStep"){
						tvIter = 6;
				}else if(algoName == "pixelLessStep"){
						tvIter = 7;
				}else if(algoName == "tobTecStep"){
						tvIter = 8;
				}else{
						tvIter = 9;
				}

				vector<pair<TrackingParticleRef, double> > tp1;
				if(recSimColl.find(trackRef1) != recSimColl.end()) tp1 = recSimColl[trackRef1];
				if(tp1.size() > 0)
				{
						tvFake = 0;
				}

				//                tvMvaVal = -99999;
				if(doMVA_){
						for(unsigned int i = 0; i < tmvaReaders_.size();i++){
								mvaValues->push_back(tmvaReaders_[i]->EvaluateMVA("BDTG"));
						}
				}

				if(makeMVATree_) outTree->Fill();
		}
}


// ------------ method called once each job just before starting event loop  ------------
		void 
trackingMVA_skimer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
		void 
trackingMVA_skimer::endJob() 
{
		outFile->cd();
		outTree->Write();
		simTree->Write();
		outFile->Close();
}

int trackingMVA_skimer::getBestVertex(TrackBaseRef track, VertexCollection vertices)
{
		//Point p(0,0,-99999);
		//Point p_dz(0,0,-99999);
		int idx1 = -1, idx2 = -1, ii=0;
		float bestWeight = 0;
		float dzmin = 10000;
		bool weightMatch = false;

		for(auto const & vertex : vertices)
		{
				float w = vertex.trackWeight(track);
				Point v_pos = vertex.position();
				if(w > bestWeight)
				{
						//p = v_pos;
						idx1 = ii;
						bestWeight = w;
						weightMatch = true;
				}
				float dz = fabs(track.get()->dz(v_pos));
				if(dz < dzmin)
				{
						//p_dz = v_pos;
						idx2 = ii;
						dzmin = dz;
				}
				ii++;
		}
		if(weightMatch) return idx1;
		else return idx2;
}

bool trackingMVA_skimer::sortVertex(const reco::Vertex & a, const reco::Vertex & b){
		if( a.tracksSize() != b.tracksSize() )
				return  a.tracksSize() > b.tracksSize() ? true : false ;
		else
				return  a.chi2() < b.chi2() ? true : false ;  
}

bool trackingMVA_skimer::hiTrkCuts(const reco::Track & track, const reco::Vertex & vertex){

		// the track cuts defined here:
		float dxyErrMax_ =3.0, dzErrMax_ = 3.0, ptErrMax_=.10, chi2nMax_ = .15;
		int nhitsMin_ = 11;
	
		math::XYZPoint vtxPoint(0.0,0.0,0.0);
		double vzErr =0.0, vxErr=0.0, vyErr=0.0;
		vtxPoint=vertex.position();
		vzErr=vertex.zError();
		vxErr=vertex.xError();
		vyErr=vertex.yError();

		double dxy=0.0, dz=0.0, dxysigma=0.0, dzsigma=0.0;
		dxy = track.dxy(vtxPoint);
		dz = track.dz(vtxPoint);
		dxysigma = sqrt(track.d0Error()*track.d0Error()+vxErr*vyErr);
		dzsigma = sqrt(track.dzError()*track.dzError()+vzErr*vzErr);

		double chi2n = track.normalizedChi2();
		double nlayers = track.hitPattern().trackerLayersWithMeasurement();
		int nhits = track.numberOfValidHits();
		//int algo  = track.algo(); 
		if(int(track.quality(reco::TrackBase::qualityByName("highPurity"))) != 1)
				return false;
		if(fabs(dxy/dxysigma) > dxyErrMax_) return false;
		if(fabs(dz/dzsigma) > dzErrMax_) return false;
		if(fabs(track.ptError()) / track.pt() > ptErrMax_) return false;
		if(nhits < nhitsMin_ ) return false;
//		int count = 0;
//		for(unsigned i = 0; i < algoParameters_.size(); i++){
//				if( algo == algoParameters_[i] ) count++;
//		}
//		if( count == 0 ) return false;
		chi2n = chi2n/nlayers;
		if(chi2n > chi2nMax_ ) return false;  

		return true;
}

void trackingMVA_skimer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{

}
//------------------------------------------------------------
//------------------------------------------------------------
void trackingMVA_skimer::endRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
		/* no op */
}

DEFINE_FWK_MODULE(trackingMVA_skimer);
