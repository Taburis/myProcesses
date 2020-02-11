
#include "myProcesses/jtc/plugin/jtcFastProducer.C"

class bjtcProducer: public jtcFastProducer{
	enum jetType {inclGenJet,inclRecoJet, trueBRecoJet, trueBGenJet};
	enum trkType {inclGenTrk,inclRecoTrk};
	public : bjtcProducer(eventMap *em):jtcFastProducer(em){}
		 bjtcProducer(){}
		 ~bjtcProducer(){}
		virtual bool evtCut(eventMap *em) override {
			if(em->hiBin >= centbins[nCent] || em->hiBin < centbins[0]) return 1;
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
			 addJtcSet("incl_GenJet_GenTrk", "inclJetSet",jetType::inclGenJet, trkType::inclGenTrk);
			 addJtcSet("trueB_GenJet_GenTrk", "trueBSet", jetType::trueBGenJet, trkType::inclGenTrk);
			 //addJtcSet("trueB_RecoJet_GenTrk", "trueBSet", jetType::trueBRecoJet, trkType::inclGenTrk);
			 //addJtcSet("trueB_RecoJet_RecoTrk", "trueBSet", jetType::trueBRecoJet, trkType::inclRecoTrk);
			 addJetQASet("trueBGenJet", jetType::trueBGenJet);
			 addJetQASet("inclGenJet", jetType::inclGenJet);
		 } ;
		 virtual void genParticleSelection(std::vector<candidate>&cands, eventMap *em) override{
			 cands.reserve(em->nGP());
			 for(int i=0; i< em->nGP(); ++i){
				 if(genParticleCuts(em, i)) continue;
				 xTagger tag;
				 tag.addTag(trkType::inclGenTrk);
				 candidate cc(tag, em->gppt(i),em->gpeta(i),em->gpphi(i),1);
				 cands.emplace_back(cc);
			 }
		 }

		 virtual void genJetSelection(std::vector<candidate>&cands, eventMap *em)override{
			 // if the jets have n types, need to add n times...
			 cands.reserve(em->nGenJet());
			 for(int i=0; i< em->nGenJet(); ++i){
				 if(genJetCuts(em, i)) continue;
				 //add incl jet tag
				 float weight = isMC ? jetWeight(em->genjetpt[i],em->genjeteta[i],em->genjetphi[i]) : 1;
				 xTagger tag; tag.addTag(jetType::inclGenJet);

				 //add true b jet tag
				 int index = em->genMatchIndex[i];
				 if(index > -1) if(TMath::Abs(em->flavor_forb[index]) == 5) 
					 tag.addTag(jetType::trueBGenJet);
				 candidate cc2(tag, em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
				 cands.emplace_back(cc2);
				 //-------------
			 }
		 };

		 histCase inclCase, trueBCase;
		bool pthat40Filter = 0;
};


