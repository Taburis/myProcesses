
#include "myProcesses/jtc/plugin/jtcFastProducer.C"

class bjtcProducer: public jtcFastProducer{
	enum jetType {inclJet, trueBJet};
	enum trkType {inclTrk};
	public : bjtcProducer(eventMap *em):jtcFastProducer(em){}
		 ~bjtcProducer(){}
		virtual bool evtCut(eventMap *em) override {
			if(em->checkEventFilter()) return 1;
			if(TMath::Abs(em->vz) > 15) return 1;
			return 0;
		}
		virtual bool mixEvtCut(eventMap *em) override {
			if( evtCut(em) ) return 1;
			if(pthat40Filter) if(em->pthat > 40) return 1;
			return 0;
		}
		 virtual void beginJob() override {
			 quickHistReg("incl_GenJet", "GenTrack", hm, inclCase, nPt, nCent);
			 quickHistReg("incl_GenJet", "RecoTrack", hm, inclCase, nPt, nCent, 1);
			 quickHistReg("trueB_GenJet", "GenTrack", hm, trueBCase, nPt, nCent);
			 quickHistReg("trueB_GenJet", "RecoTrack", hm, trueBCase, nPt, nCent,1);
			 xTagger inclJetTag(jetType::inclJet), trueBJetTag(jetType::trueBJet), inclTrkTag(trkType::inclTrk);
			 regJtcPair(inclJetTag, inclTrkTag, inclCase);
			 regJtcPair(trueBJetTag, inclTrkTag, trueBCase);
		 } ;
		 virtual void genParticleSelections(std::vector<candidate>&cands, eventMap *em) override{
			 cands.reserve(em->nGP());
			 for(int i=0; i< em->nGP(); ++i){
				 if(genParticleCuts(em, i)) continue;
				 xTagger tag;
				 tag.addTag(trkType::inclTrk);
				 candidate cc(tag, em->gppt(i),em->gpeta(i),em->gpphi(i),1);
				 cands.emplace_back(cc);
			 }
		 }

		 virtual void genJetSelections(std::vector<candidate>&cands, eventMap *em)override{
			 // if the jets have n types, need to add n times...
			 cands.reserve(em->nGenJet());
			 for(int i=0; i< em->nGenJet(); ++i){
				 if(genJetCuts(em, i)) continue;
				 //add incl jet tag
				 float weight = isMC ? jetWeight(em->genjetpt[i],em->genjeteta[i],em->genjetphi[i]) : 1;
				 xTagger tag; tag.addTag(jetType::inclJet);

				 //add true b jet tag
				 int index = em->genMatchIndex[i];
				 if(index > -1) if(TMath::Abs(em->flavor_forb[index]) == 5) 
					 tag.addTag(jetType::trueBJet);
				 candidate cc2(tag, em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
				 cands.emplace_back(cc2);
				 //-------------
			 }
		 };

		 histCase inclCase, trueBCase;
		bool pthat40Filter = 0;
};


