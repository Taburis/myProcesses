
#include "myProcesses/jtc/plugin/jtcFastProducer.C"

class bjtcProducer: public jtcFastProducer{
	enum jetType {inclJet, trueBJet};
	enum trkType {inclTrk};
	public : bjtcProducer(eventMap *em):jtcFastProducer(em){}
		 ~bjtcProducer(){}
		 virtual void beginJob() override {
			 xTagger inclJetTag(jetType::inclJet), trueBJetTag(jetType::trueBJet), inclTrkTag(trkType::inclTrk);
			 quickHistReg("inclJet", "GenJet_GenTrack", hm, inclCase, nPt, nCent);
			 quickHistReg("trueBJet", "GenJet_GenTrack", hm, trueBCase, nPt, nCent);
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
				 float weight = isMC ? jetPtWeight(em->genjetpt[i]) : 1;
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
};


