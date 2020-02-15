
#include "myProcesses/jtc/plugin/jtcFastProducer.C"

class bjtcProducer: public jtcFastProducer{
	enum jetType {inclJet, trueBJet, taggedJet};
	enum trkType {inclTrk};
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
			 if(isMC) if(pthat40Filter) if(em->pthat > 40) return 1;
			 return 0;
		 }
		 virtual void beginJob() override {
			 xTagger inclJtTg(jetType::inclJet);
			 xTagger trueBJtTg(jetType::trueBJet);
			 xTagger taggedJtTg(jetType::taggedJet);
			 xTagger tagTrueJtTg(jetType::taggedJet); 
			 tagTrueJtTg.addTag(jetType::trueBJet);
			 xTagger inclTrkTg(trkType::inclTrk);

			 addJetQASet("tagged", taggedJtTg);
			 addJtcSet("tagged", taggedJtTg, inclTrkTg);
			 addJetQASet("incl", inclJtTg);
			 addJtcSet("incl"  , inclJtTg, inclTrkTg);
			 if(!isMC) return ;
			 addJetQASet("tagTrue", tagTrueJtTg);
			 addJtcSet("tagTrue", tagTrueJtTg, inclTrkTg);
			 addJetQASet("trueB", trueBJtTg);
			 addJtcSet("trueB" , trueBJtTg, inclTrkTg);
		 } ;
		 virtual void genParticleSelection(std::vector<candidate>&cands, eventMap *em) override{
			 cands.reserve(em->nGP());
			 for(int i=0; i< em->nGP(); ++i){
				 if(genParticleCuts(em, i)) continue;
				 xTagger tag;
				 tag.addTag(trkType::inclTrk);
				 candidate cc(tag, 0, em->gppt(i),em->gpeta(i),em->gpphi(i),1);
				 cands.emplace_back(cc);
			 }
		 }
		 virtual void trkSelection(std::vector<candidate>&cands, eventMap *em)override{
			 cands.reserve(em->nTrk());
			 for(int i=0; i< em->nTrk(); ++i){
				 if(recoTrkCuts(em, i)) continue;
				 xTagger tag(trkType::inclTrk);
				 candidate cc(tag, 1, em->trkpt[i],em->trketa[i],em->trkphi[i],1);
				 cands.emplace_back(cc);
			 }
		 }
		 virtual void recoJetSelection(std::vector<candidate>&cands, eventMap *em)override{
			 cands.reserve(em->nJet());
			 for(int i=0; i< em->nJet(); ++i){
				 if(recoJtCuts(em, i)) continue;
				 float weight = isMC ? jetWeight(em->jetpt[i],em->jeteta[i],em->jetphi[i]) : 1;
				 xTagger tag; tag.addTag(jetType::inclJet);
				 if(em->disc_csvV2[i] > csv_cut) tag.addTag(jetType::taggedJet);
				 if(isMC) if(TMath::Abs(em->flavor_forb[i]) == 5){
					 tag.addTag(jetType::trueBJet);
				 }
				 candidate cc2(tag,1,em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
				 cands.emplace_back(cc2);
			 }
		 }

		 virtual void genJetSelection(std::vector<candidate>&cands, eventMap *em)override{
			 // if the jets have n types, need to add n times...
			 cands.reserve(em->nGenJet());
			 for(int i=0; i< em->nGenJet(); ++i){
				 if(genJetCuts(em, i)) continue;
				 //add incl jet tag
				 float weight = isMC ? jetWeight(em->genjetpt[i],em->genjeteta[i],em->genjetphi[i]) : 1;
				 xTagger tag; tag.addTag(jetType::inclJet);

				 //add true b jet tag
				 int index = em->genMatchIndex[i];
				 if(index > -1){
					 if(TMath::Abs(em->flavor_forb[index]) == 5) 
						 tag.addTag(jetType::trueBJet);
					 if(em->disc_csvV2[index] > csv_cut) 
						 tag.addTag(jetType::taggedJet);
				 }
				 candidate cc2(tag,0,em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
				 cands.emplace_back(cc2);
				 //-------------
			 }
		 };


		 histCase inclCase, trueBCase;
		 bool pthat40Filter = 0;
		 xTagger trueBTag, tagBTag, tagTrueBTag;
		 float csv_cut = 0.9;
};


