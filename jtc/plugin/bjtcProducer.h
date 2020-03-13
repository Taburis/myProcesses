
#include "myProcesses/jtc/plugin/jtcFastProducer.C"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/JEC2018PbPb/JECorr.h"
#include "myProcesses/jtc/JEC2018PbPb/JECUncert.h"
#include <vector>

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
			 if(isMC && em->pthat < 50) return 1;
			 return 0;
		 }
		 virtual bool mixEvtCut(eventMap *em) override {
			 if( evtCut(em) ) return 1;
			 if(isMC) if(pthat40Filter) if(em->pthat > 40) return 1;
			 return 0;
		 }
		 virtual void beginJob() override {
			 xTagger inclJtTg, trueBJtTg, taggedJtTg, tagTrueJtTg, inclTrkTg;
			 inclJtTg   .addTag(jetType::inclJet);
			 trueBJtTg  .addTag(jetType::trueBJet);
			 taggedJtTg .addTag(jetType::taggedJet);
			 tagTrueJtTg.addTag(jetType::taggedJet); 
			 tagTrueJtTg.addTag(jetType::trueBJet);
			 inclTrkTg  .addTag(trkType::inclTrk);

			 addJtcSet("incl"  , inclJtTg, inclTrkTg);
			 addJtcSet("tagged", taggedJtTg, inclTrkTg);
			 if(!isMC) return ;
			 addJtcSet("tagTrue", tagTrueJtTg, inclTrkTg);
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
		 bool recoTrkCuts(eventMap *em, int j) override{
			 //return 1 to skip
			 if(em->trkpt[j] < 1 || em->trkpt[j]>400) return 1;
			 if(TMath::Abs(em->trketa[j]) >= 2.4) return 1;
			 if(!(em->highPurity[j])) return 1;
			 if(em->trknhit[j]< 11) return 1;
			 if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > 0.15) return 1;
			 float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
			 if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j]))) return 1;
			 return 0;
		 }
		 virtual void trkSelection(std::vector<candidate>&cands, eventMap *em)override{
			 cands.reserve(em->nTrk());
			 for(int i=0; i< em->nTrk(); ++i){
				 if(recoTrkCuts(em, i)) continue;
				 xTagger tag; tag.addTag(trkType::inclTrk);
				 candidate cc(tag, 1, em->trkpt[i],em->trketa[i],em->trkphi[i],1);
				 cands.emplace_back(cc);
			 }
		 }
		 virtual void recoJetSelection(std::vector<candidate>&cands, eventMap *em)override{
			 cands.reserve(em->nJet());
			 for(int i=0; i< em->nJet(); ++i){
				 if(recoJtCuts(em, i)) continue;
				 float jetpt = em->jetpt[i];
				 if(addJEC)  jetpt = get_correctedPt(em,i);
				 float weight = isMC ? jetWeight(jetpt,em->jeteta[i],em->jetphi[i]) : 1;
				 xTagger tag; tag.addTag(jetType::inclJet);
				 if(em->disc_csvV2[i] > csv_cut) tag.addTag(jetType::taggedJet);
				 if(isMC) if(TMath::Abs(em->flavor_forb[i]) == 5){
					 tag.addTag(jetType::trueBJet);
				 }
				 candidate cc2(tag,1,jetpt, em->jet_wta_eta[i], em->jet_wta_phi[i], weight);
				 cands.emplace_back(cc2);
			 }
		 }
		 virtual bool recoJtCuts(eventMap *em, int j) override {
			 //return 1 to skip
			 float jetpt = em->jetpt[j];
			 if(addJEC)  jetpt = get_correctedPt(em,j);
			 if( jetpt < jtpt_min) return 1;
			 if(TMath::Abs(em->jeteta[j]) > jteta_max) return 1;
			 return 0;
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
				 int index = -1;
				 float ptdiff = 20, drmin=1;
				 for(int j=0; j<em->nJet(); ++j){
					 float dr = findDr(em->genjeteta[i],em->genjetphi[i],em->jeteta[j],em->jetphi[j]);
					 if(dr > 0.4) continue;
					 if(dr < drmin && fabs(em->genjetpt[i]-em->jetpt[j]) > ptdiff ) {
						 drmin = dr; index = j;
					 }
				 }
				 //int index = em->genMatchIndex[i];
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

		 double get_correctedPt(eventMap *em, int ijet){
			 JEC.SetJetPT(em->jetpt[ijet]);
			 JEC.SetJetEta(em->jeteta[ijet]);
			 JEC.SetJetPhi(em->jetphi[ijet]);
			 return JEC.GetCorrectedPT();
		 }

		 void loadJEC(){
			 jecFiles.emplace_back("../JEC2018PbPb/Autumn18_HI_V6_DATA_L2Relative_AK4PF.txt");
			 jecFiles.emplace_back("../JEC2018PbPb/Autumn18_HI_V6_DATA_L2L3Residual_AK4PF.txt");
			 addJEC = 1;
			 JEC.Initialize(jecFiles);
		 }

		 histCase inclCase, trueBCase;
		 bool pthat40Filter = 0;
		 xTagger trueBTag, tagBTag, tagTrueBTag;
		 float csv_cut = 0.9;
		 float jtpt_min = 120.0, jteta_max = 1.6;
		 bool addJEC = 1;
		 JetCorrector JEC;
		 JetUncertainty JEU;
		 std::vector<string> jecFiles;
};
