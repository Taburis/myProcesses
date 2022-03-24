#ifndef trackingMVA_skimer_h
#define trackingMVA_skimer_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"
#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit1D.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "SimDataFormats/Associations/interface/TrackToTrackingParticleAssociator.h"
#include "DataFormats/Math/interface/Point3D.h"
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <map>

#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TString.h"

#include "TMVA/Reader.h"

namespace edm
{
	class Event;
	class EventSetup;
	class ParameterSet;
}

class trackingMVA_skimer : public edm::EDAnalyzer {
	public:
		explicit trackingMVA_skimer(const edm::ParameterSet& Params);
		~trackingMVA_skimer();
		typedef math::XYZPoint Point;
		static bool sortVertex( const reco::Vertex &  a, const reco::Vertex & b );

	private:
		virtual void beginJob();
		virtual void analyze(const edm::Event&, const edm::EventSetup&);
		void beginRun(const edm::Run&, const edm::EventSetup&);
		void endRun(const edm::Run&, const edm::EventSetup&);
		virtual void endJob();

		int getBestVertex(reco::TrackBaseRef,reco::VertexCollection);
		bool hiTrkCuts(const reco::Track & trk, const reco::Vertex & vtx);

		const reco::TrackToTrackingParticleAssociator* associator;
		std::string source;
		std::string associatorName;
		TFile* outFile;
		edm::InputTag simSource;
		edm::InputTag beamspot_;
		edm::EDGetTokenT<reco::VertexCollection> vertices;
		edm::EDGetTokenT<reco::BeamSpot> beamspotToken;
		edm::EDGetTokenT<reco::TrackToTrackingParticleAssociator> associatorToken;
		edm::EDGetTokenT<edm::View<reco::Track> > trackToken;
		edm::EDGetTokenT<edm::View<reco::Track> > trackHighPurityToken;
		edm::EDGetTokenT<TrackingParticleCollection> simTPToken;


		float tvFake;
		float tvIter;
		float tvNdof;
		float tvpt;
		float tvNlayers;
                float tvNlayers3D;
		float tvNlayersLost;
                float tvChi2n;
                float tvChi2n_no1Dmod;
                float tvEta;
                //float tvPhi;
                float tvRelPtErr;
                float tvNhits;
                float tvLostIn;
                float tvLostOut;
                float tvMinLost;
                float tvLostMidFrac;
                float tvAbsDz;
                float tvAbsD0;
                float tvAbsDzPV;
                float tvAbsD0PV;
                float tvMvaVal;
		Int_t isHP;


		Int_t sim_rec;
		bool  passTrkCut=0;
		float sim_pt;
		float sim_eta;
		float sim_phi;

		bool doMVA_, makeSimTree_, makeMVATree_;
                std::vector<TMVA::Reader*> tmvaReaders_;
                std::vector<std::string> mvaTypes_;
//		std::string mvaType_;
                std::vector<float> d_mvaValues;
                std::vector<float>* mvaValues;

                TTree* outTree, *simTree;

                edm::ESHandle<MagneticField> magfield;
};

class trackCompare
{
	public:
		bool operator()(const reco::Track t1, const reco::Track t2){return t1.innerMomentum().Rho() > t2.innerMomentum().Rho();}
};

#endif
