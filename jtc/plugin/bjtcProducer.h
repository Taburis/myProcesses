
#include "myProcesses/jtc/plugin/jtcFastProducer.C"
#include "myProcesses/jtc/plugin/jtcUti.h"
#include "myProcesses/jtc/JEC2018PbPb/JECorr.h"
#include "myProcesses/jtc/JEC2018PbPb/JECUncert.h"
#include <vector>
#include "TRandom.h"

class bjtcProducer: public jtcFastProducer{
	enum jetType {inclJet, trueBJet, taggedJet, negTagJet, cjet};
	enum trkType {inclTrk, sube0,suben0};
	public : bjtcProducer(eventMap *em):jtcFastProducer(em){}
		 bjtcProducer(){}
		 ~bjtcProducer(){}
		 virtual bool evtCut(eventMap *em) override {
			 if(em->hiBin >= centbins[nCent] || em->hiBin < centbins[0]) return 1;
			 if(em->checkEventFilter()) return 1;
			 if(TMath::Abs(em->vz) > 15) return 1;
			 if(isMC) if( em->pthat < 50) return 1;
			 //if(isMC) if( em->pthat < 40) return 1;
			 return 0;
		 }
		 virtual bool mixEvtCut(eventMap *em) override {
			 if( evtCut(em) ) return 1;
			 if(isMC) if(pthat40Filter) if(em->pthat > 40) return 1;
			 return 0;
		 }
		 void addJtcSetForSube(TString name, xTagger jetTg, bool domix){
			 addJetQASet(name, jetTg);
			 addJtcSet(name+"_sube0_RecoJet_GenTrk",name+"_RecoJet_GenTrk", jetTg, 1, sube0TrkTg, 0,domix);
			 addJtcSet(name+"_sube0_GenJet_GenTrk" ,name+"_GenJet_GenTrk" , jetTg, 0, sube0TrkTg, 0,domix);
			 addJtcSet(name+"_subeN0_RecoJet_GenTrk",name+"_RecoJet_GenTrk", jetTg, 1, subeNTrkTg, 0, domix);
			 addJtcSet(name+"_subeN0_GenJet_GenTrk" ,name+"_GenJet_GenTrk" , jetTg, 0, subeNTrkTg, 0, domix);
			 //addJtcSet(name+"_GenJet_GenTrk" , name+"_GenJet_GenTrk"  , jetTg, 0, inclTrkTg, 0, domix);
			 //addJtcSet(name+"_RecoJet_GenTrk" ,name+"_RecoJet_GenTrk" , jetTg, 1, inclTrkTg, 0, domix);
			 //addJtcSet(name+"_RecoJet_RecoTrk",name+"_RecoJet_RecoTrk", jetTg, 1, inclTrkTg, 1, domix);
		 }
		 virtual void beginJob() override {
			 inclJtTg   .addTag(jetType::inclJet);
			 trueBJtTg  .addTag(jetType::trueBJet);
			 taggedJtTg .addTag(jetType::taggedJet);
			 tagTrueJtTg.addTag(jetType::taggedJet); 
			 tagTrueJtTg.addTag(jetType::trueBJet);
			 inclTrkTg  .addTag(trkType::inclTrk);
			 negTagJtTg .addTag(jetType::negTagJet);
			 cJtTg      .addTag(jetType::cjet);
			 sube0TrkTg .addTag(trkType::sube0);
			 subeNTrkTg .addTag(trkType::suben0);

//			 if(dosube) domixing = 0;

			 if(dosube && isMC){
				 addJtcSetForSube("incl", inclJtTg, domixing);
				 addJtcSetForSube("tagged", taggedJtTg, domixing);
				 //addJtcSetForSube("negTag", negTagJtTg, domixing);
				 addJtcSetForSube("tagTrue", tagTrueJtTg,domixing);
				 addJtcSetForSube("trueB" , trueBJtTg, domixing);
			 }else{
				 addJtcSet("incl"  , inclJtTg, inclTrkTg);
				 addJtcSet("tagged", taggedJtTg, inclTrkTg);
				 addJtcSet("negTag", negTagJtTg, inclTrkTg);
				 if(!isMC) return ;
				 addJtcSet("tagTrue", tagTrueJtTg, inclTrkTg);
				 addJtcSet("trueB" , trueBJtTg, inclTrkTg);
			 }
		 };
		 virtual void genParticleSelection(std::vector<candidate>&cands, eventMap *em) override{
			 cands.reserve(em->nGP());
			 for(int i=0; i< em->nGP(); ++i){
				 if(genParticleCuts(em, i)) continue;
				 xTagger tag;
				 tag.addTag(trkType::inclTrk);
				 if(em->gpSube(i) ==0 ) tag.addTag(trkType::sube0);
				 else tag.addTag(trkType::suben0);
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
				 if(em->ndisc_csvV2[i] > csv_cut){
					 tag.addTag(jetType::negTagJet);
				 }
				 if(em->disc_csvV2[i] > csv_cut) tag.addTag(jetType::taggedJet);
				 if(isMC) if(TMath::Abs(em->flavor_forb[i]) == 5){
					 tag.addTag(jetType::trueBJet);
				 }
				 if(isMC) if(TMath::Abs(em->flavor_forb[i]) == 5){
					 tag.addTag(jetType::cjet);
				 }
				 candidate cc2(tag,1,jetpt, em->jet_wta_eta[i], em->jet_wta_phi[i], weight);
				 cands.emplace_back(cc2);
			 }
		 }
		 virtual bool recoJtCuts(eventMap *em, int j) override {
			 //return 1 to skip
			 float jetpt = em->jetpt[j];
			 if(doJEU)    jetpt = get_pt_JEU(em,j);
			 if(doJERSmear)  jetpt = jetpt*random.Gaus(1, 0.024); //smearing 20% worse
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
					 if(em->ndisc_csvV2[index] > csv_cut){ 
						 tag.addTag(jetType::negTagJet);
}
					 if(TMath::Abs(em->flavor_forb[index]) == 4) 
						 tag.addTag(jetType::cjet);
				 }
				 candidate cc2(tag,0,em->genjetpt[i], em->genjet_wta_eta[i], em->genjet_wta_phi[i], weight);
				 cands.emplace_back(cc2);
				 //-------------
			 }
		 };

		 double get_pt_JEU(eventMap *em, int ijet){
			float pt = em->jetpt[ijet];
			 JEU.SetJetPT(em->jetpt[ijet]);
			 JEU.SetJetEta(em->jeteta[ijet]);
			 JEU.SetJetPhi(em->jetphi[ijet]);
			 if(jecDown) return pt*(1-JEU.GetUncertainty().first);
			 else return pt*(1+JEU.GetUncertainty().second);
		 }

		 double get_correctedPt(eventMap *em, int ijet){
			 JEC.SetJetPT(em->jetpt[ijet]);
			 JEC.SetJetEta(em->jeteta[ijet]);
			 JEC.SetJetPhi(em->jetphi[ijet]);
			 return JEC.GetCorrectedPT();
		 }

		 void loadJEC(){
			 jecFiles.emplace_back("myProcesses/jtc/JEC2018PbPb/Autumn18_HI_V6_DATA_L2Relative_AK4PF.txt");
			 jecFiles.emplace_back("myProcesses/jtc/JEC2018PbPb/Autumn18_HI_V6_DATA_L2L3Residual_AK4PF.txt");
			 addJEC = 1;
			 JEC.Initialize(jecFiles);
		 }
		 void loadJEU(){
			 doJEU=1;
			JEU.Initialize("myProcesses/jtc/JEC2018PbPb/Autumn18_HI_V6_DATA_Uncertainty_AK4PF.txt");
		 }
		 void setJEU_Up(){ loadJEU(); jecUp=1; jecDown=0;}
		 void setJEU_Down(){ loadJEU(); jecUp=0; jecDown=1;}

		 xTagger sube0TrkTg, subeNTrkTg;
		 xTagger inclJtTg, trueBJtTg, taggedJtTg, negTagJtTg, tagTrueJtTg, inclTrkTg, cJtTg;

		 histCase inclCase, trueBCase;
		 bool pthat40Filter = 0, dosube=0, jecUp=0, jecDown=0, doJEU=0;
		 float csv_cut = 0.9;
		 float jtpt_min = 120.0, jteta_max = 1.6;
		 bool addJEC = 0, doJERSmear = 0;
		 TRandom random;
		 JetCorrector JEC;
		 JetUncertainty JEU;
		 std::vector<string> jecFiles;
};
